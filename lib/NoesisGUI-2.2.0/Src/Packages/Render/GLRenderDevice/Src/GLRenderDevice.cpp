////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


//#undef NS_MINIMUM_LOG_LEVEL
//#define NS_MINIMUM_LOG_LEVEL 0


#include "GLRenderDevice.h"

#include <NsRender/Texture.h>
#include <NsRender/RenderTarget.h>
#include <NsCore/Ptr.h>
#include <NsCore/Log.h>
#include <NsCore/StringUtils.h>

#include <NsMath/Utils.h>
#include <EASTL/algorithm.h>

#include <stdio.h>

#if NS_RENDERER_USE_NSGL
    #include <OpenGL/CGLCurrent.h>
    #include <dlfcn.h>
#endif


using namespace Noesis;
using namespace NoesisApp;


#define NS_GL_VER(major, minor) (major * 10 + minor)

#ifdef NS_DEBUG
    #define V(exp) \
        NS_MACRO_BEGIN \
            while (glGetError() != GL_NO_ERROR); \
            exp; \
            GLenum err = glGetError(); \
            if (err != GL_NO_ERROR) \
            { \
                switch (err) \
                { \
                    case GL_INVALID_ENUM: NS_FATAL("%s [GL_INVALID_ENUM]", #exp); \
                    case GL_INVALID_VALUE: NS_FATAL("%s [GL_INVALID_VALUE]", #exp); \
                    case GL_INVALID_OPERATION: NS_FATAL("%s [GL_INVALID_OPERATION]", #exp); \
                    case GL_INVALID_FRAMEBUFFER_OPERATION: NS_FATAL("%s [GL_INVALID_FRAMEBUFFER_OPERATION]", #exp); \
                    case GL_OUT_OF_MEMORY: NS_FATAL("%s [GL_OUT_OF_MEMORY]", #exp); \
                    default: NS_FATAL("%s [0x%08x]", #exp, err); \
                } \
            } \
        NS_MACRO_END
#else
    #define V(exp) exp
#endif

#ifndef DYNAMIC_VB_SIZE
    #define DYNAMIC_VB_SIZE 512 * 1024
#endif

#ifndef DYNAMIC_IB_SIZE
    #define DYNAMIC_IB_SIZE 128 * 1024
#endif

#ifdef NS_PROFILE
    #define GL_PUSH_GROUP_MARKER(n) PushGroupMarker(0, n)
    #define GL_POP_GROUP_MARKER() PopGroupMarker()
#else
    #define GL_PUSH_GROUP_MARKER(n) NS_NOOP
    #define GL_POP_GROUP_MARKER() NS_NOOP
#endif

#ifdef NS_COMPILER_MSVC
#define sscanf sscanf_s
#endif


const uint32_t VFPos = 0;
const uint32_t VFColor = 1;
const uint32_t VFTex0 = 2;
const uint32_t VFTex1 = 4;
const uint32_t VFCoverage = 8;

const uint32_t AlphaMask = 0x00001;
const uint32_t RedMask = 0x00002;
const uint32_t GreenMask = 0x00004;
const uint32_t BlueMask = 0x00008;

// Default OpenGL texture parameters
const SamplerState DefaultSampler = 
{
    {
        WrapMode::Repeat,
        MinMagFilter::Linear,
        MipFilter::Nearest
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
class GLTexture final: public Texture
{
public:
    GLTexture(GLRenderDevice* device_, uint32_t width_, uint32_t height_, uint32_t levels_,
        GLenum format_, bool isInverted_): device(device_), width(width_), height(height_),
        levels(levels_), format(format_), isInverted(isInverted_), object(0),
        state(DefaultSampler) {}

    GLTexture(GLuint object_, uint32_t width_, uint32_t height_, uint32_t levels_, GLenum format_,
        bool isInverted_): device(0), width(width_), height(height_), levels(levels_),
        format(format_), isInverted(isInverted_), object(object_), state(DefaultSampler) {}

    ~GLTexture()
    {
        // We don't have ownership of textures created by WrapTexture()
        if (device != 0)
        {
            device->DeleteTexture(this);
        }
    }

    uint32_t GetWidth() const override { return width; }
    uint32_t GetHeight() const override { return height; }
    bool HasMipMaps() const override { return levels > 1; }
    bool IsInverted() const override { return isInverted; }

    GLRenderDevice* const device;

    const uint32_t width;
    const uint32_t height;
    const uint32_t levels;
    const GLenum format;
    const bool isInverted;

    GLuint object;
    SamplerState state;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
class GLRenderTarget final: public RenderTarget
{
public:
    GLRenderTarget(GLRenderDevice* device_, uint32_t width_, uint32_t height_, GLsizei samples_):
        device(device_), width(width_), height(height_), samples(samples_), fbo(0), fboResolved(0),
        stencil(0), colorAA(0)
    {
        texture = *new GLTexture(device, width, height, 1, GL_RGBA, true);
    }

    ~GLRenderTarget()
    {
        texture.Reset();
        NS_ASSERT(device != 0);
        device->DeleteRenderTarget(this);
    }

    Texture* GetTexture() override { return texture; }

    GLRenderDevice* const device;

    const uint32_t width;
    const uint32_t height;
    const GLsizei samples;

    GLuint fbo;
    GLuint fboResolved;

    GLuint stencil;
    GLuint colorAA;

    Ptr<GLTexture> texture;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
GLRenderDevice::GLRenderDevice(): mGLVersion(0), mDefaultVAO(0)
{
    BindAPI();
    FindExtensions();
    FillCaps();

    CreateSamplers();
    CreatePrograms();
    CreateVertexFormats();

    InvalidateStateCache();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLRenderDevice::~GLRenderDevice()
{
    if (mDefaultVAO != 0)
    {
        DeleteVertexArrays(1, &mDefaultVAO);
    }

    for (uint32_t i = 0; i < NS_COUNTOF(mVertexFormats); i++)
    {
        if (mVertexFormats[i].vao != 0)
        {
            DeleteVertexArrays(1, &mVertexFormats[i].vao);
        }
    }

    DestroyBuffer(mDynamicVB);
    DestroyBuffer(mDynamicIB);

    for (uint32_t i = 0; i < NS_COUNTOF(mSamplers); i++)
    {
        if (mSamplers[i] != 0)
        {
            DeleteSampler(mSamplers[i]);
        }
    }

    for (uint32_t i = 0; i < NS_COUNTOF(mPrograms); i++)
    {
        if (mPrograms[i].object != 0)
        {
            DeleteProgram(mPrograms[i].object);
        }
    }

#if NS_RENDERER_USE_WGL
    FreeLibrary(mOpenGL32);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<Texture> GLRenderDevice::WrapTexture(GLuint object, uint32_t width, uint32_t height,
    uint32_t levels, bool isInverted)
{
    return *new GLTexture(object, width, height, levels, GL_RGBA, isInverted);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const DeviceCaps& GLRenderDevice::GetCaps() const
{
    return mCaps;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<RenderTarget> GLRenderDevice::CreateRenderTarget(const char* label, uint32_t width,
    uint32_t height, uint32_t sampleCount)
{
    NS_ASSERT(sampleCount > 0);
    NS_LOG_TRACE("CreateRenderTarget '%s'", label);

    // samples must be 0 to disable multisampling in glRenderbufferStorageMultisample
    sampleCount = eastl::min_alt(sampleCount, MaxSamples());
    GLsizei samples = sampleCount == 1 ? 0 : sampleCount;

    Ptr<GLRenderTarget> surface = *new GLRenderTarget(this, width, height, samples);

    // Texture creation
    GLenum format = HaveTexStorage() ? GL_RGBA8 : GL_RGBA;
    InitTexture(surface->texture, format, GL_RGBA);

    if (samples != 0 && !HaveMultisampledRenderToTexture())
    {
        surface->colorAA = CreateColor(width, height, samples);
    }

    surface->stencil = CreateStencil(width, height, samples);

    CreateFBO(samples, surface->texture->object, surface->stencil, surface->colorAA, surface->fbo,
        surface->fboResolved);

    return surface;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<RenderTarget> GLRenderDevice::CloneRenderTarget(const char* label, RenderTarget* shared_)
{
    NS_LOG_TRACE("CreateRenderTarget '%s'", label);

    GLRenderTarget* shared = (GLRenderTarget*)(shared_);
    GLsizei samples = shared->samples;
    uint32_t width = shared->width;
    uint32_t height = shared->height;

    Ptr<GLRenderTarget> surface = *new GLRenderTarget(this, width, height, samples);

    // Texture creation
    GLenum format = HaveTexStorage() ? GL_RGBA8 : GL_RGBA;
    InitTexture(surface->texture, format, GL_RGBA);

    CreateFBO(samples, surface->texture->object, shared->stencil, shared->colorAA, surface->fbo,
        surface->fboResolved);

    return surface;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<Texture> GLRenderDevice::CreateTexture(const char* label, uint32_t width, uint32_t height,
    uint32_t numLevels, TextureFormat::Enum format_)
{
    NS_LOG_TRACE("CreateTexture '%s'", label);

    GLenum internalFormat;
    GLenum format;

    GetGLFormat(format_, internalFormat, format);
    Ptr<GLTexture> texture = *new GLTexture(this, width, height, numLevels, format, false);
    InitTexture(texture, internalFormat, format);

    return texture;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::UpdateTexture(Texture* texture_, uint32_t level, uint32_t x, uint32_t y,
    uint32_t width, uint32_t height, const void* data)
{
    ActiveTexture(0);

    GLTexture* texture = (GLTexture*)(texture_);
    BindTexture2D(texture->object);

    TexSubImage2D(level, x, y, width, height, texture->format, data);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::BeginRender(bool offscreen)
{
    NS_UNUSED(offscreen);
    GL_PUSH_GROUP_MARKER(offscreen ? "Noesis.Offscreen": "Noesis");
    InvalidateStateCache();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::SetRenderTarget(RenderTarget* surface)
{
    GL_PUSH_GROUP_MARKER("Framebuffer");
    BindRenderTarget((GLRenderTarget*)(surface));

    // As we cannot fully clear the surface we try hinting the driver to avoid unnecessary loads.
    // Unfortunately in iOS this is not working and the driver is loading color and stencil.
    // This is even worse with multisampling. So, in mobiles avoid multisampled offscreen surfaces.
    // TODO: detect GL_QCOM_tiled_rendering extension to improve this
    GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_STENCIL_ATTACHMENT };
    DiscardFramebuffer(GL_FRAMEBUFFER, 2, attachments);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::BeginTile(const Tile& tile, uint32_t surfaceWidth, uint32_t surfaceHeight)
{
    NS_UNUSED(surfaceWidth);
    NS_UNUSED(surfaceHeight);
    GL_PUSH_GROUP_MARKER("Tile");

    EnableScissor();
    Scissor(tile.x, tile.y, tile.width, tile.height);

    EnableColor();
    SetClearColor(0);
    SetClearStencil(0);
    V(glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::EndTile()
{
    GL_POP_GROUP_MARKER();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::ResolveRenderTarget(RenderTarget* surface, const Tile* tiles, uint32_t numTiles)
{
    Resolve((GLRenderTarget*)surface, tiles, numTiles);
    GL_POP_GROUP_MARKER();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::EndRender()
{
    UnbindObjects();
    GL_POP_GROUP_MARKER();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void* GLRenderDevice::MapVertices(uint32_t bytes)
{
    return MapBuffer(mDynamicVB, bytes);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::UnmapVertices()
{
    UnmapBuffer(mDynamicVB);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void* GLRenderDevice::MapIndices(uint32_t bytes)
{
    return MapBuffer(mDynamicIB, bytes);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::UnmapIndices()
{
    UnmapBuffer(mDynamicIB);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::DrawBatch(const Batch& batch)
{
    NS_ASSERT(batch.shader.v < NS_COUNTOF(mPrograms));
    uint8_t vertexFormat = mPrograms[batch.shader.v].vertexFormat;
    SetVertexFormat(vertexFormat, mDynamicVB.drawPos + batch.vertexOffset);
    SetTextures(batch);

    SetRenderState(GLRenderState(batch.renderState, batch.stencilRef));
    ActivateProgram(batch);

    NS_ASSERT(GetInteger(GL_ELEMENT_ARRAY_BUFFER_BINDING) == (GLint)mDynamicIB.boundObject);
    const GLvoid *indices = (GLvoid*)(uintptr_t)(mDynamicIB.drawPos + 2 * batch.startIndex);
    V(glDrawElements(GL_TRIANGLES, batch.numIndices, GL_UNSIGNED_SHORT, indices));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::DeleteTexture(GLTexture* texture)
{
    GLuint object = texture->object;

    if (object != 0)
    {
        for (uint32_t i = 0; i < NS_COUNTOF(mBoundTextures); i++)
        {
            if (mBoundTextures[i] == object)
            {
                mBoundTextures[i] = 0;
            }
        }

        DeleteTexture(object);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::DeleteRenderTarget(GLRenderTarget* surface)
{
    if (mBoundRenderTarget == surface)
    {
        mBoundRenderTarget = 0;
    }

    if (surface->stencil != 0)
    {
        DeleteRenderbuffer(surface->stencil);
    }

    if (surface->colorAA != 0)
    {
        DeleteRenderbuffer(surface->colorAA);
    }

    if (surface->fbo != 0)
    {
        DeleteFramebuffer(surface->fbo);
    }

    if (surface->fboResolved != 0)
    {
        DeleteFramebuffer(surface->fboResolved);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::InvalidateStateCache()
{
    mViewportWidth = 0xffffffff;
    mViewportHeight = 0xffffffff;

    mScissorX = 0xffffffff;
    mScissorY = 0xffffffff;
    mScissorWidth = 0xffffffff;
    mScissorHeight = 0xffffffff;

    mClearColor = 0xffffffffffffffff;
    mClearStencil = 0xffffffff;

    mVertexArray = 0xffffffff;
    mActiveTextureUnit = 0xffffffff;
    mDynamicVB.boundObject = 0xffffffff;
    mDynamicIB.boundObject = 0xffffffff;
    mProgram = 0xffffffff;
    mEnabledVertexAttribArrays = 0xffffffff;
    mBoundRenderTarget = 0;

    memset(mBoundTextures, 0, sizeof(mBoundTextures));
    memset(mBoundSamplers, 0, sizeof(mBoundSamplers));

    mRenderState.f.invalidated = true;
    mUnpackStateSet = false;

    V(glDisable(GL_CULL_FACE));
    V(glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
    V(glBlendEquation(GL_FUNC_ADD));
    V(glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE));
    V(glDisable(GL_SAMPLE_COVERAGE));
    V(glDisable(GL_DEPTH_TEST));
    V(glDepthMask(GL_FALSE));
    V(glStencilMask(0xff));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::UnbindObjects()
{
    if (BindVertexArray_ != 0)
    {
        V(BindVertexArray_(0));
        mVertexArray = 0;
    }

    V(glBindBuffer(GL_ARRAY_BUFFER, 0));
    mDynamicVB.boundObject = 0;

    V(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    mDynamicIB.boundObject = 0;

    for (uint32_t i = 0; i < TextureUnit::Count; i++)
    {
        V(glActiveTexture(GL_TEXTURE0 + i));
        V(glBindTexture(GL_TEXTURE_2D, 0));
        mBoundTextures[i] = 0;

        if (IsSupported(Extension::ARB_sampler_objects))
        {
            V(glBindSampler(i, 0));
            mBoundSamplers[i] = 0;
        }
    }

    mActiveTextureUnit = TextureUnit::Count - 1;

    V(glUseProgram(0));
    mProgram = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::CreateBuffer(DynamicBuffer& buffer, GLenum target, uint32_t size) const
{
    buffer.target = target;
    buffer.size = size;
    buffer.pos = 0;
    buffer.drawPos = 0;
    buffer.index = 0;

    if (!HaveMapBufferRange())
    {
        buffer.cpuMemory = Alloc(size);
    }
    else
    {
        buffer.cpuMemory = nullptr;
    }

    V(glGenBuffers(NS_COUNTOF(buffer.objects), buffer.objects));
    for (uint32_t i = 0; i < NS_COUNTOF(buffer.objects); i++)
    {
        V(glBindBuffer(target, buffer.objects[i]));
        V(glBufferData(target, size, 0, GL_STREAM_DRAW));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::DestroyBuffer(DynamicBuffer& buffer) const
{
    V(glDeleteBuffers(NS_COUNTOF(buffer.objects), buffer.objects));
    Dealloc(buffer.cpuMemory);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::BindBuffer(DynamicBuffer& buffer)
{
    if (HaveVertexArrayObject())
    {
        BindVertexArray(mDefaultVAO);
    }

    if (buffer.objects[buffer.index] != buffer.boundObject)
    {
        NS_ASSERT(glIsBuffer(buffer.objects[buffer.index]));
        V(glBindBuffer(buffer.target, buffer.objects[buffer.index]));
        buffer.boundObject = buffer.objects[buffer.index];
    }

    NS_ASSERT(buffer.boundObject == buffer.objects[buffer.index]);
    NS_ASSERT(GetInteger(buffer.target + 2) == GLint(buffer.objects[buffer.index]));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::SetVertexFormat(uint8_t format, uintptr_t offset)
{
    NS_ASSERT(format < NS_COUNTOF(mVertexFormats));

#if !NS_RENDERER_USE_EMS
#if GL_VERSION_4_3 || GL_ES_VERSION_3_1 || GL_ARB_vertex_attrib_binding
    if (IsSupported(Extension::ARB_vertex_attrib_binding))
    {
        BindVertexArray(mVertexFormats[format].vao);
        V(glBindVertexBuffer(0, mDynamicVB.boundObject, offset, mVertexFormats[format].stride));
        V(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mDynamicIB.boundObject));
    }
    else
#endif
#endif
    {
        NS_ASSERT(GetInteger(GL_ARRAY_BUFFER_BINDING) == (GLint)mDynamicVB.boundObject);
        GLsizei stride = mVertexFormats[format].stride;

        V(glVertexAttribPointer(Attr::Pos, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset));
        offset += 8;

        if ((format & VFColor) > 0)
        {
            GLenum type = GL_UNSIGNED_BYTE;
            V(glVertexAttribPointer(Attr::Color, 4, type, GL_TRUE, stride, (void*)offset));
            offset += 4;
        }

        if ((format & VFTex0) > 0)
        {
            V(glVertexAttribPointer(Attr::Tex0, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset));
            offset += 8;
        }

        if ((format & VFTex1) > 0)
        {
            V(glVertexAttribPointer(Attr::Tex1, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset));
            offset += 8;
        }

        if ((format & VFCoverage) > 0)
        {
            V(glVertexAttribPointer(Attr::Coverage, 1, GL_FLOAT, GL_FALSE, stride, (void*)offset));
        }

        EnableVertexAttribArrays((format << 1) | 1);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::DisableClientState() const
{
#if NS_RENDERER_OPENGL
    // If external code enabled any of these client states (like Unity is doing in OSX) then our
    // Vertex Buffers are ignored and a slow path is followed inside the GL driver
    if (mGLVersion < NS_GL_VER(3,0))
    {
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_EDGE_FLAG_ARRAY);
        glDisableClientState(GL_FOG_COORD_ARRAY);
        glDisableClientState(GL_INDEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::EnableVertexAttribArrays(uint32_t state)
{
    if (mEnabledVertexAttribArrays == 0xffffffff)
    {
        mEnabledVertexAttribArrays = ~state;

        if (mDefaultVAO == 0)
        {
            DisableClientState();

            for (uint32_t i = Attr::Count; i < mMaxVertexAttribs; i++)
            {
                V(glDisableVertexAttribArray(i));
            }
        }
    }

    uint32_t delta = state ^ mEnabledVertexAttribArrays;
    if (delta != 0)
    {
        for (uint32_t i = 0; i < Attr::Count; i++)
        {
            uint32_t mask = 1 << i;
            if (delta & mask)
            {
                if (state & mask)
                {
                    V(glEnableVertexAttribArray(i));
                }
                else
                {
                    V(glDisableVertexAttribArray(i));
                }
            }
        }

        mEnabledVertexAttribArrays = state;
    }

#ifdef NS_DEBUG
    for (uint32_t i = 0; i < Attr::Count; i++)
    {
        NS_ASSERT(VertexAttribArrayEnabled(i) == ((state & (1 << i)) > 0));
    }
    for (uint32_t i = Attr::Count; i < mMaxVertexAttribs; i++)
    {
        NS_ASSERT(!VertexAttribArrayEnabled(i));
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void* GLRenderDevice::MapBuffer(DynamicBuffer& buffer, uint32_t size)
{
    NS_ASSERT(size <= buffer.size);
    NS_ASSERT(buffer.pos <= buffer.size);

    if (HaveMapBufferRange())
    {
        GLbitfield access = GL_MAP_WRITE_BIT;
        buffer.drawPos = buffer.pos;

    // Buffer orphaning and progressives updates in Android are unsafe because the access bit field
    // and update ranges can be ignored generating CPU stalls. We prefer to use a n-buffer scheme.
    // It is simpler and less buggy. Similar performance although less memory efficient.
#if 0
        if (size <= buffer.size - buffer.pos)
        {
            access |= GL_MAP_UNSYNCHRONIZED_BIT;
        }
        else
#endif
        {
            access |= GL_MAP_INVALIDATE_BUFFER_BIT;
            buffer.drawPos = buffer.pos = 0;
            buffer.index = buffer.index == NS_COUNTOF(buffer.objects) - 1 ? 0 : buffer.index + 1;
        }
    
        buffer.pos += size;
        BindBuffer(buffer);

        return MapBufferRange(buffer.target, buffer.drawPos, size, access);
    }
    else
    {
        buffer.pos = size;
        return buffer.cpuMemory;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::UnmapBuffer(DynamicBuffer& buffer)
{
    if (HaveMapBufferRange())
    {
        UnmapBuffer(buffer.target);
    }
    else
    {
        BindBuffer(buffer);
        V(glBufferSubData(buffer.target, 0, buffer.pos, buffer.cpuMemory));
        buffer.index = buffer.index == NS_COUNTOF(buffer.objects) - 1 ? 0 : buffer.index + 1;
    }
}

namespace
{
#if NS_RENDERER_OPENGL
    namespace GL21
    {
        #include "ShadersGL120.h"
    }
    namespace GL31
    {
        #include "ShadersGL140.h"
    }
#else
    #include "ShadersGLES.h"
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::CreateSamplers()
{
    if (IsSupported(Extension::ARB_sampler_objects))
    {
        memset(mSamplers, 0, sizeof(mSamplers));

        for (uint8_t minmag = 0; minmag < MinMagFilter::Count; minmag++)
        {
            for (uint8_t mip = 0; mip < MipFilter::Count; mip++)
            {
                for (uint8_t uv = 0; uv < WrapMode::Count; uv++)
                {
                    SamplerState s = {{uv, minmag, mip}};

                    NS_ASSERT(s.v < NS_COUNTOF(mSamplers));
                    V(glGenSamplers(1, &mSamplers[s.v]));

                    V(glSamplerParameteri(mSamplers[s.v], GL_TEXTURE_WRAP_S, GLWrapSMode(s)));
                    V(glSamplerParameteri(mSamplers[s.v], GL_TEXTURE_WRAP_T, GLWrapTMode(s)));
                    V(glSamplerParameteri(mSamplers[s.v], GL_TEXTURE_MIN_FILTER, GLMinFilter(s)));
                    V(glSamplerParameteri(mSamplers[s.v], GL_TEXTURE_MAG_FILTER, GLMagFilter(s)));
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::CreatePrograms()
{
#if NS_RENDERER_OPENGL
    bool isGL31 = mGLVersion >= NS_GL_VER(3,1);
    #define VSHADER(n) CompileShader(GL_VERTEX_SHADER, isGL31 ? GL31::n : GL21::n)
    #define FSHADER(n) CompileShader(GL_FRAGMENT_SHADER, isGL31 ? GL31::n : GL21::n)
#else
    #define VSHADER(n) CompileShader(GL_VERTEX_SHADER, n)
    #define FSHADER(n) CompileShader(GL_FRAGMENT_SHADER, n)
#endif

    GLuint vShaders[] = 
    {
        VSHADER(Pos_VS),
        VSHADER(PosColor_VS),
        VSHADER(PosTex0_VS),
        VSHADER(PosColorCoverage_VS),
        VSHADER(PosTex0Coverage_VS),
        VSHADER(PosColorTex1_VS),
        VSHADER(PosTex0Tex1_VS)
    };

    GLuint fShaders[] = 
    {
        FSHADER(RGBA_FS),
        FSHADER(Mask_FS),
        FSHADER(PathSolid_FS),
        FSHADER(PathLinear_FS),
        FSHADER(PathRadial_FS),
        FSHADER(PathPattern_FS),
        FSHADER(PathAASolid_FS),
        FSHADER(PathAALinear_FS),
        FSHADER(PathAARadial_FS),
        FSHADER(PathAAPattern_FS),
        FSHADER(ImagePaintOpacitySolid_FS),
        FSHADER(ImagePaintOpacityLinear_FS),
        FSHADER(ImagePaintOpacityRadial_FS),
        FSHADER(ImagePaintOpacityPattern_FS),
        FSHADER(TextSolid_FS),
        FSHADER(TextLinear_FS),
        FSHADER(TextRadial_FS),
        FSHADER(TextPattern_FS),
    };

    struct Program
    {
        uint8_t vertexFormat;
        int8_t vShaderIdx;
        int8_t fShaderIdx;
    };

    const Program programs[] =
    {
        { VFPos, 0, 0 }, // RGBA
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { VFPos, 0, 1 }, // Mask
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { VFPos|VFColor, 1, 2 }, // PathSolid
        { VFPos|VFTex0, 2, 3 }, // PathLinear
        { VFPos|VFTex0, 2, 4 }, // PathRadial
        { VFPos|VFTex0, 2, 5 }, // PathPattern
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { VFPos|VFColor|VFCoverage, 3, 6 }, // PathAASolid
        { VFPos|VFTex0|VFCoverage, 4, 7 }, // PathAALinear
        { VFPos|VFTex0|VFCoverage, 4, 8 }, // PathAARadial
        { VFPos|VFTex0|VFCoverage, 4, 9 }, // PathAAPattern
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { VFPos|VFColor|VFTex1, 5, 10 }, // ImagePaintOpacitySolid
        { VFPos|VFTex0|VFTex1, 6, 11 }, // ImagePaintOpacityLinear
        { VFPos|VFTex0|VFTex1, 6, 12 }, // ImagePaintOpacityRadial
        { VFPos|VFTex0|VFTex1, 6, 13 }, // ImagePaintOpacityPattern
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { 0, -1, -1 },
        { VFPos|VFColor|VFTex1, 5, 14 }, // TextSolid
        { VFPos|VFTex0|VFTex1, 6, 15 }, // TextLinear
        { VFPos|VFTex0|VFTex1, 6, 16 }, // TextRadial
        { VFPos|VFTex0|VFTex1, 6, 17 }, // TextPattern
    };

    const char* attribs[] =
    {
        "attr_pos", "attr_color", "attr_tex0", "attr_tex1", "attr_coverage", 0
    };

    static_assert(NS_COUNTOF(mPrograms) == NS_COUNTOF(programs), "");

    for (uint32_t i = 0; i < NS_COUNTOF(programs); i++)
    {
        const Program& p = programs[i];

        if (p.vShaderIdx != -1 && p.fShaderIdx != -1)
        {
            GLuint po = LinkProgram(vShaders[p.vShaderIdx], fShaders[p.fShaderIdx], attribs);

            BindTextureLocation(po, "pattern", TextureUnit::Pattern);
            BindTextureLocation(po, "ramps", TextureUnit::Ramps);
            BindTextureLocation(po, "image", TextureUnit::Image);
            BindTextureLocation(po, "glyphs", TextureUnit::Glyphs);

            mPrograms[i].vertexFormat = p.vertexFormat;
            mPrograms[i].object = po;

            mPrograms[i].projectionMtxLocation = glGetUniformLocation(po, "projectionMtx");
            mPrograms[i].projectionMtxHash = 0;

            mPrograms[i].opacityLocation = glGetUniformLocation(po, "opacity");
            mPrograms[i].opacityHash = 0;

            mPrograms[i].radialGradLocation = glGetUniformLocation(po, "radialGrad");
            mPrograms[i].radialGradHash = 0;

            mPrograms[i].rgbaLocation = glGetUniformLocation(po, "rgba");
            mPrograms[i].rgbaHash = 0;
        }
        else
        {
            mPrograms[i].object = 0;
        }
    }

    for (uint32_t i = 0; i < NS_COUNTOF(vShaders); i++)
    {
        DeleteShader(vShaders[i]);
    }

    for (uint32_t i = 0; i < NS_COUNTOF(fShaders); i++)
    {
        DeleteShader(fShaders[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::CreateVertexFormats()
{
    CreateBuffer(mDynamicVB, GL_ARRAY_BUFFER, DYNAMIC_VB_SIZE);
    CreateBuffer(mDynamicIB, GL_ELEMENT_ARRAY_BUFFER, DYNAMIC_IB_SIZE);

    mMaxVertexAttribs = GetInteger(GL_MAX_VERTEX_ATTRIBS);

    if (BindVertexArray_ != 0)
    {
        GenVertexArrays(1, &mDefaultVAO);
        V(BindVertexArray_(mDefaultVAO));
    }

#if !NS_RENDERER_USE_EMS
#if GL_VERSION_4_3 || GL_ES_VERSION_3_1 || GL_ARB_vertex_attrib_binding
    if (IsSupported(Extension::ARB_vertex_attrib_binding))
    {
        NS_ASSERT(BindVertexArray_ != 0);

        for (uint32_t i = 0; i < NS_COUNTOF(mVertexFormats); i++)
        {
            GenVertexArrays(1, &mVertexFormats[i].vao);
            V(BindVertexArray_(mVertexFormats[i].vao));

            GLuint offset = 0;
            V(glEnableVertexAttribArray(Attr::Pos));
            V(glVertexAttribBinding(Attr::Pos, 0));
            V(glVertexAttribFormat(Attr::Pos, 2, GL_FLOAT, GL_FALSE, offset));
            offset += 8;

            if ((i & VFColor) > 0)
            {
                V(glEnableVertexAttribArray(Attr::Color));
                V(glVertexAttribBinding(Attr::Color, 0));
                V(glVertexAttribFormat(Attr::Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, offset));
                offset += 4;
            }

            if ((i & VFTex0) > 0)
            {
                V(glEnableVertexAttribArray(Attr::Tex0));
                V(glVertexAttribBinding(Attr::Tex0, 0));
                V(glVertexAttribFormat(Attr::Tex0, 2, GL_FLOAT, GL_FALSE, offset));
                offset += 8;
            }

            if ((i & VFTex1) > 0)
            {
                V(glEnableVertexAttribArray(Attr::Tex1));
                V(glVertexAttribBinding(Attr::Tex1, 0));
                V(glVertexAttribFormat(Attr::Tex1, 2, GL_FLOAT, GL_FALSE, offset));
                offset += 8;
            }

            if ((i & VFCoverage) > 0)
            {
                V(glEnableVertexAttribArray(Attr::Coverage));
                V(glVertexAttribBinding(Attr::Coverage, 0));
                V(glVertexAttribFormat(Attr::Coverage, 1, GL_FLOAT, GL_FALSE, offset));
                offset += 4;
            }

            mVertexFormats[i].stride = offset;
        }
    }
    else
#endif
#endif
    {
        for (uint32_t i = 0; i < NS_COUNTOF(mVertexFormats); i++)
        {
            mVertexFormats[i].vao = 0;

            GLuint stride = 8;
            stride += (i & VFColor) > 0 ? 4 : 0;
            stride += (i & VFTex0) > 0 ? 8 : 0;
            stride += (i & VFTex1) > 0 ? 8 : 0;
            stride += (i & VFCoverage) > 0 ? 4 : 0;

            mVertexFormats[i].stride = stride;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLboolean GLRenderDevice::HaveMapBufferRange() const
{
    return MapBufferRange_ != 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLvoid* GLRenderDevice::MapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length,
    GLbitfield access) const
{
    NS_ASSERT(MapBufferRange_ != 0);
    GLvoid* ptr;
    V(ptr = MapBufferRange_(target, offset, length, access));
    return ptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::UnmapBuffer(GLenum target) const
{
    NS_ASSERT(UnmapBuffer_ != 0);
    V(UnmapBuffer_(target));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLboolean GLRenderDevice::HaveVertexArrayObject() const
{
    return GenVertexArrays_ != 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::GenVertexArrays(GLsizei n, GLuint *arrays) const
{
    NS_ASSERT(GenVertexArrays_ != 0);
    V(GenVertexArrays_(n, arrays));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::BindVertexArray(GLuint name)
{
    NS_ASSERT(BindVertexArray_ != 0);
    NS_ASSERT(name == 0 || IsVertexArray(name));

    if (name != mVertexArray)
    {
        V(BindVertexArray_(name));
        mVertexArray = name;
    }

    NS_ASSERT(GetInteger(GL_VERTEX_ARRAY_BINDING) == GLint(name));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::DeleteVertexArrays(GLsizei n, const GLuint *arrays) const
{
    NS_ASSERT(DeleteVertexArrays_ != 0);
    V(DeleteVertexArrays_(n, arrays));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLboolean GLRenderDevice::IsVertexArray(GLuint array) const
{
    NS_ASSERT(IsVertexArray_ != 0);
    return IsVertexArray_(array);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLuint GLRenderDevice::RenderbufferStorage(GLsizei samples, GLenum format, GLsizei width,
    GLsizei height) const
{
    GLuint buffer;
    V(glGenRenderbuffers(1, &buffer));
    V(glBindRenderbuffer(GL_RENDERBUFFER, buffer));
    RenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
    return buffer;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::RenderbufferStorageMultisample(GLenum target, GLsizei samples,
    GLenum internalformat, GLsizei width, GLsizei height) const
{
    NS_ASSERT(samples <= (GLsizei)MaxSamples());

    if (samples == 0 || RenderbufferStorageMultisample_ == 0)
    {
        // At least one device (Mali-400) gives GL_INVALID_OPERATION if we dont follow this path
        // In this device EXT_multisampled_render_to_texture is enabled but GL_MAX_SAMPLES is 0
        V(glRenderbufferStorage(target, internalformat, width, height));
    }
    else
    {
        V(RenderbufferStorageMultisample_(target, samples, internalformat, width, height));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLboolean GLRenderDevice::HaveMultisampledRenderToTexture() const
{
    return FramebufferTexture2DMultisample_ != 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::FramebufferTexture2DMultisample(GLenum target, GLenum attachment,
    GLenum textarget, GLuint texture, GLint level, GLsizei samples) const
{
    NS_ASSERT(FramebufferTexture2DMultisample_ != 0);
    V(FramebufferTexture2DMultisample_(target, attachment, textarget, texture, level, samples));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLuint GLRenderDevice::CreateStencil(GLsizei width, GLsizei height, GLsizei samples) const
{
#if NS_RENDERER_OPENGL
    bool haveS8 = IsSupported(Extension::ARB_ES3_compatibility);
    GLenum format = haveS8 ? GL_STENCIL_INDEX8 : GL_DEPTH24_STENCIL8;
#else
    GLenum format = GL_STENCIL_INDEX8;
#endif

    NS_LOG_TRACE("Stencil %d x %d %dx (0x%x)", width, height, samples > 0 ? samples : 1, format);
    return RenderbufferStorage(samples, format, width, height);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLuint GLRenderDevice::CreateColor(GLsizei width, GLsizei height, GLsizei samples) const
{
#if NS_RENDERER_OPENGL
    GLenum format = GL_RGBA8;
#else
    GLenum format = IsSupported(Extension::OES_rgb8_rgba8) ? GL_RGBA8: GL_RGB565;
#endif

    NS_LOG_TRACE("Color %d x %d %dx (0x%x)", width, height, samples, format);
    return RenderbufferStorage(samples, format, width, height);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::CreateFBO(GLsizei samples, GLuint texture, GLuint stencil, GLuint colorAA,
    GLuint& fbo, GLuint& fboResolved) const
{
    GLint prevFBO;
    V(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO));
  
    V(glGenFramebuffers(1, &fbo));
    V(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

    const GLenum FB = GL_FRAMEBUFFER;
    const GLenum COLOR0 = GL_COLOR_ATTACHMENT0;

    // Draw Color Buffer
    if (samples == 0)
    {
        NS_ASSERT(colorAA == 0);
        V(glFramebufferTexture2D(FB, COLOR0, GL_TEXTURE_2D, texture, 0));
    }
    else if (HaveMultisampledRenderToTexture())
    {
        NS_ASSERT(colorAA == 0);
        FramebufferTexture2DMultisample(FB, COLOR0, GL_TEXTURE_2D, texture, 0, samples);
    }
    else
    {
        NS_ASSERT(colorAA != 0);
        V(glFramebufferRenderbuffer(FB, COLOR0, GL_RENDERBUFFER, colorAA));
    }

    // Stencil Buffer
#if NS_RENDERER_OPENGL
    bool haveS8 = IsSupported(Extension::ARB_ES3_compatibility);
    GLenum attachment = haveS8 ? GL_STENCIL_ATTACHMENT : GL_DEPTH_STENCIL_ATTACHMENT;
#else
    GLenum attachment = GL_STENCIL_ATTACHMENT;
#endif
    V(glFramebufferRenderbuffer(FB, attachment, GL_RENDERBUFFER, stencil));

    NS_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    // Resolve framebuffer
    if (colorAA != 0)
    {
        V(glGenFramebuffers(1, &fboResolved));
        V(glBindFramebuffer(GL_FRAMEBUFFER, fboResolved));
        V(glFramebufferTexture2D(FB, COLOR0, GL_TEXTURE_2D, texture, 0));
        NS_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    }

    V(glBindFramebuffer(GL_FRAMEBUFFER, prevFBO));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::Resolve(GLRenderTarget* target, const Tile* tiles, uint32_t numTiles)
{
    NS_ASSERT(GetInteger(GL_FRAMEBUFFER_BINDING) == (GLint)target->fbo);

    if (target->fboResolved != 0)
    {
        V(glBindFramebuffer(GL_READ_FRAMEBUFFER, target->fbo));
        V(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target->fboResolved));
        mBoundRenderTarget = 0;

        // It seems most (iOS) implementations of BlitFrameBuffer actually obey the scissor test
        EnableScissor();

        for (uint32_t i = 0; i < numTiles; i++)
        {
            const Tile& tile = tiles[i];
            Scissor(tile.x, tile.y, tile.width, tile.height);

#if NS_RENDERER_USE_EAGL
            if (IsSupported(Extension::APPLE_framebuffer_multisample))
            {
                V(glResolveMultisampleFramebufferAPPLE());
            }
            else
#endif
            {
                GLint x0 = tile.x;
                GLint y0 = tile.y;
                GLint x1 = tile.x + tile.width;
                GLint y1 = tile.y + tile.height;

                NS_ASSERT(HaveBlitFramebuffer());
                BlitFramebuffer(x0, y0, x1, y1, x0, y0, x1, y1, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            }
        }

        GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_STENCIL_ATTACHMENT };
        DiscardFramebuffer(GL_READ_FRAMEBUFFER, 2, attachments);
    }
    else
    {
        GLenum attachments[] = { GL_STENCIL_ATTACHMENT };
        DiscardFramebuffer(GL_FRAMEBUFFER, 1, attachments);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::BindRenderTarget(GLRenderTarget* renderTarget)
{
    if (renderTarget != mBoundRenderTarget)
    {
        mBoundRenderTarget = renderTarget;

        if (mBoundRenderTarget != 0)
        {
            NS_ASSERT(mBoundRenderTarget->fbo == 0 || glIsFramebuffer(mBoundRenderTarget->fbo));
            V(glBindFramebuffer(GL_FRAMEBUFFER, mBoundRenderTarget->fbo));
            Viewport(mBoundRenderTarget->width, mBoundRenderTarget->height);
        }
    }

    NS_ASSERT(!renderTarget || GetInteger(GL_FRAMEBUFFER_BINDING) == (GLint)renderTarget->fbo);
    NS_ASSERT(!renderTarget || GetInteger(GL_VIEWPORT, 0) == (GLint)0);
    NS_ASSERT(!renderTarget || GetInteger(GL_VIEWPORT, 1) == (GLint)0);
    NS_ASSERT(!renderTarget || GetInteger(GL_VIEWPORT, 2) == (GLint)renderTarget->width);
    NS_ASSERT(!renderTarget || GetInteger(GL_VIEWPORT, 3) == (GLint)renderTarget->height);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::DiscardFramebuffer(GLenum target, GLsizei numAttachments,
    const GLenum *attachments) const
{
    if (InvalidateFramebuffer_ != 0)
    {
        V(InvalidateFramebuffer_(target, numAttachments, attachments));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool GLRenderDevice::HaveBlitFramebuffer() const
{
    return BlitFramebuffer_ != 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::BlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
    GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) const
{
    NS_ASSERT(BlitFramebuffer_ != 0);
    V(BlitFramebuffer_(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::DeleteFramebuffer(GLuint framebuffer) const
{
    NS_ASSERT(glIsFramebuffer(framebuffer));
    V(glDeleteFramebuffers(1, &framebuffer));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::DeleteRenderbuffer(GLuint renderbuffer) const
{
    NS_ASSERT(glIsRenderbuffer(renderbuffer));
    V(glDeleteRenderbuffers(1, &renderbuffer));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool GLRenderDevice::HaveBorderClamp() const
{
    return IsSupported(Extension::ARB_texture_border_clamp) || IsSupported(
        Extension::EXT_texture_border_clamp);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool GLRenderDevice::HaveTexStorage() const
{
    return TexStorage2D_ != 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool GLRenderDevice::HaveTexRG() const
{
    return IsSupported(Extension::ARB_texture_rg) || IsSupported(Extension::EXT_texture_rg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::GetGLFormat(TextureFormat::Enum texFormat, GLenum& internalFormat,
    GLenum& format)
{
    switch (texFormat)
    {
        case TextureFormat::RGBA8:
        {
            internalFormat = HaveTexStorage() ? GL_RGBA8 : GL_RGBA;
            format = GL_RGBA;
            break;
        }
        case TextureFormat::R8:
        {
            if (HaveTexRG())
            {
                internalFormat = HaveTexStorage() ? GL_R8 : GL_RED;
                format = GL_RED;
            }
            else
            {
                internalFormat = HaveTexStorage() ? GL_LUMINANCE8 : GL_LUMINANCE;
                format = GL_LUMINANCE;
            }
            break;
        }
        default:
            NS_ASSERT_UNREACHABLE;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLint GLRenderDevice::GLWrapSMode(SamplerState sampler) const
{
    switch (sampler.f.wrapMode)
    {
        case WrapMode::ClampToEdge:
        {
            return GL_CLAMP_TO_EDGE;
        }
        case WrapMode::ClampToZero:
        {
            return HaveBorderClamp() ? GL_CLAMP_TO_BORDER : GL_CLAMP_TO_EDGE;
        }
        case WrapMode::Repeat:
        {
            return GL_REPEAT;
        }
        case WrapMode::MirrorU:
        {
            return GL_MIRRORED_REPEAT;
        }
        case WrapMode::MirrorV:
        {
            return GL_REPEAT;
        }
        case WrapMode::Mirror:
        {
            return GL_MIRRORED_REPEAT;
        }
        default:
            NS_ASSERT_UNREACHABLE;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLint GLRenderDevice::GLWrapTMode(SamplerState sampler) const
{
    switch (sampler.f.wrapMode)
    {
        case WrapMode::ClampToEdge:
        {
            return GL_CLAMP_TO_EDGE;
            break;
        }
        case WrapMode::ClampToZero:
        {
            return HaveBorderClamp() ? GL_CLAMP_TO_BORDER : GL_CLAMP_TO_EDGE;
            break;
        }
        case WrapMode::Repeat:
        {
            return GL_REPEAT;
            break;
        }
        case WrapMode::MirrorU:
        {
            return GL_REPEAT;
            break;
        }
        case WrapMode::MirrorV:
        {
            return GL_MIRRORED_REPEAT;
            break;
        }
        case WrapMode::Mirror:
        {
            return GL_MIRRORED_REPEAT;
            break;
        }
        default:
            NS_ASSERT_UNREACHABLE;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLint GLRenderDevice::GLMinFilter(SamplerState sampler) const
{
    switch (sampler.f.minmagFilter)
    {
        case MinMagFilter::Nearest:
        {
            switch (sampler.f.mipFilter)
            {
                case MipFilter::Disabled:
                {
                    return GL_NEAREST;
                }
                case MipFilter::Nearest:
                {
                    return GL_NEAREST_MIPMAP_NEAREST;
                }
                case MipFilter::Linear:
                {
                    return GL_NEAREST_MIPMAP_LINEAR;
                }
                default:
                {
                    NS_ASSERT_UNREACHABLE;
                }
            }
        }
        case MinMagFilter::Linear:
        {
            switch (sampler.f.mipFilter)
            {
                case MipFilter::Disabled:
                {
                    return GL_LINEAR;
                }
                case MipFilter::Nearest:
                {
                    return GL_LINEAR_MIPMAP_NEAREST;
                }
                case MipFilter::Linear:
                {
                    return GL_LINEAR_MIPMAP_LINEAR;
                }
                default:
                {
                    NS_ASSERT_UNREACHABLE;
                }
            }
        }
        default:
        {
            NS_ASSERT_UNREACHABLE;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLint GLRenderDevice::GLMagFilter(SamplerState sampler) const
{
    switch (sampler.f.minmagFilter)
    {
        case MinMagFilter::Nearest:
        {
            return GL_NEAREST;
        }
        case MinMagFilter::Linear:
        {
            return GL_LINEAR;
        }
        default:
            NS_ASSERT_UNREACHABLE;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::TexStorage2D(GLenum target, GLsizei levels, GLenum internalformat,
    GLsizei width, GLsizei height)
{
    NS_ASSERT(TexStorage2D_ != 0);
    V(TexStorage2D_(target, levels, internalformat, width, height));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::InitTexture(GLTexture* texture, GLenum internalFormat, GLenum format)
{
    V(glGenTextures(1, &texture->object));

    SamplerState state = texture->state;
    uint32_t levels = texture->levels;

    if (levels == 1)
    {
        // Hint the driver to avoid allocating all the mipmaps
        state.f.mipFilter = MipFilter::Disabled;
    }

    ActiveTexture(0);
    BindTexture2D(texture->object);
    SetTextureState(texture, state);

    if (IsSupported(Extension::APPLE_texture_max_level))
    {
        V(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levels - 1));
    }

    uint32_t width = texture->width;
    uint32_t height = texture->height;

    NS_LOG_TRACE("Texture %d x %d x %d (0x%x)", width, height, levels, internalFormat);

    if (!HaveTexStorage())
    {
        for (uint32_t i = 0; i < levels; i++)
        {
            V(glTexImage2D(GL_TEXTURE_2D, i, internalFormat, width, height, 0, format,
                GL_UNSIGNED_BYTE, 0));
            width = eastl::max_alt(1U, width / 2);
            height = eastl::max_alt(1U, height / 2);
        }
    }
    else
    {
        TexStorage2D(GL_TEXTURE_2D, levels, internalFormat, width, height);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::SetTextureState(GLTexture* texture, SamplerState state)
{
    if (IsSupported(Extension::ARB_sampler_objects))
    {
        NS_ASSERT(state.v < NS_COUNTOF(mSamplers));
        BindSampler(mSamplers[state.v]);
    }
    else
    {
        SamplerState delta;
        delta.v = texture->state.v ^ state.v;

        if (delta.v != 0)
        {
            if (delta.f.wrapMode != 0)
            {
                V(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLWrapSMode(state)));
                V(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLWrapTMode(state)));
            }

            if (delta.f.minmagFilter != 0 || delta.f.mipFilter != 0)
            {
                V(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLMinFilter(state)));
                V(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLMagFilter(state)));
            }
        }

        texture->state = state;

        NS_ASSERT(GetTexParameter(GL_TEXTURE_WRAP_S) == GLWrapSMode(state));
        NS_ASSERT(GetTexParameter(GL_TEXTURE_WRAP_T) == GLWrapTMode(state));
        NS_ASSERT(GetTexParameter(GL_TEXTURE_MIN_FILTER) == GLMinFilter(state));
        NS_ASSERT(GetTexParameter(GL_TEXTURE_MAG_FILTER) == GLMagFilter(state));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::SetTexture(uint32_t unit, GLTexture* texture, SamplerState state)
{
    NS_ASSERT(unit < NS_COUNTOF(mBoundTextures));
    NS_ASSERT(texture->object == 0 || glIsTexture(texture->object));

    bool cached = mBoundTextures[unit] == texture->object;

    if (IsSupported(Extension::ARB_sampler_objects))
    {
        cached = cached && mSamplers[state.v] != 0 && mSamplers[state.v] == mBoundSamplers[unit];
    }
    else
    {
        // As the state of external textures is not known we set it everytime
        if (texture->device == 0)
        {
            texture->state.v = ~state.v;
        }

        cached = cached && texture->state.v == state.v;
    }

    if (!cached)
    {
        ActiveTexture(unit);
        BindTexture2D(texture->object);
        SetTextureState(texture, state);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::SetTextures(const Batch& batch)
{
    if (batch.pattern != 0)
    {
        SetTexture(TextureUnit::Pattern, (GLTexture*)batch.pattern, batch.patternSampler);
    }

    if (batch.ramps != 0)
    {
        SetTexture(TextureUnit::Ramps, (GLTexture*)batch.ramps, batch.rampsSampler);
    }

    if (batch.image != 0)
    {
        SetTexture(TextureUnit::Image, (GLTexture*)batch.image, batch.imageSampler);
    }

    if (batch.glyphs != 0)
    {
        SetTexture(TextureUnit::Glyphs, (GLTexture*)batch.glyphs, batch.glyphsSampler);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::ActiveTexture(uint32_t unit)
{
    if (mActiveTextureUnit != unit)
    {
        V(glActiveTexture(GL_TEXTURE0 + unit));
        mActiveTextureUnit = unit;
    }

    NS_ASSERT(GetInteger(GL_ACTIVE_TEXTURE) == GL_TEXTURE0 + (GLint)unit);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::BindTexture2D(GLuint texture)
{
    NS_ASSERT(mActiveTextureUnit < NS_COUNTOF(mBoundTextures));

    if (mBoundTextures[mActiveTextureUnit] != texture)
    {
        V(glBindTexture(GL_TEXTURE_2D, texture));
        mBoundTextures[mActiveTextureUnit] = texture;
    }

    NS_ASSERT(GetInteger(GL_TEXTURE_BINDING_2D) == (GLint)texture);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::SetUnpackState()
{
    if (!mUnpackStateSet)
    {
        V(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

#if NS_RENDERER_OPENGL
        V(glPixelStorei(GL_UNPACK_SWAP_BYTES, 0));
        V(glPixelStorei(GL_UNPACK_LSB_FIRST, 0));
        V(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
        V(glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0));
        V(glPixelStorei(GL_UNPACK_SKIP_ROWS, 0));
#endif

        mUnpackStateSet = true;
    }

    NS_ASSERT(GetInteger(GL_UNPACK_ALIGNMENT) == 1);

#if NS_RENDERER_OPENGL
    NS_ASSERT(GetInteger(GL_UNPACK_SWAP_BYTES) == 0);
    NS_ASSERT(GetInteger(GL_UNPACK_LSB_FIRST) == 0);
    NS_ASSERT(GetInteger(GL_UNPACK_ROW_LENGTH) == 0);
    NS_ASSERT(GetInteger(GL_UNPACK_SKIP_PIXELS) == 0);
    NS_ASSERT(GetInteger(GL_UNPACK_SKIP_ROWS) == 0);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::TexSubImage2D(GLint level, GLint xOffset, GLint yOffset, GLint width,
    GLint height, GLenum format, const GLvoid* data)
{
    SetUnpackState();
    V(glTexSubImage2D(GL_TEXTURE_2D, level, xOffset, yOffset, width, height, format,
        GL_UNSIGNED_BYTE, data));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::BindSampler(GLuint sampler)
{
    NS_ASSERT(glIsSampler(sampler));
    NS_ASSERT(mActiveTextureUnit < NS_COUNTOF(mBoundSamplers));
    NS_ASSERT(GetInteger(GL_ACTIVE_TEXTURE) == GL_TEXTURE0 + (GLint)mActiveTextureUnit);

    if (mBoundSamplers[mActiveTextureUnit] != sampler)
    {
        V(glBindSampler(mActiveTextureUnit, sampler));
        mBoundSamplers[mActiveTextureUnit] = sampler;
    }

    NS_ASSERT(GetInteger(GL_SAMPLER_BINDING) == (GLint)sampler);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::DeleteSampler(GLuint sampler) const
{
    NS_ASSERT(glIsSampler(sampler));
    V(glDeleteSamplers(1, &sampler));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::DeleteTexture(GLuint texture) const
{
    NS_ASSERT(glIsTexture(texture));
    V(glDeleteTextures(1, &texture));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLuint GLRenderDevice::CompileShader(GLenum type, const char* source) const
{
    GLuint shader = glCreateShader(type);
    V(glShaderSource(shader, 1, &source, 0));
    V(glCompileShader(shader));

    GLint status;
    V(glGetShaderiv(shader, GL_COMPILE_STATUS, &status));
    if (status == 0)
    {
        GLint len = 0;
        V(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len));
        
        if (len == 0)
        {
            V(glDeleteShader(shader));
            NS_FATAL("Shader compilation failed. No info");
        }
        else
        {
            char error[512];
            glGetShaderInfoLog(shader, 512, 0, error);
            V(glDeleteShader(shader));
            NS_FATAL("%s", error);
        }
    }
    
    return shader;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLuint GLRenderDevice::LinkProgram(GLuint vso, GLuint pso, const char** attribs) const
{
    GLuint prog = glCreateProgram();
    V(glAttachShader(prog, vso));
    V(glAttachShader(prog, pso));
    
    if (attribs != 0)
    {
        for (GLuint i = 0; attribs[i]; i++)
        {
            V(glBindAttribLocation(prog, i, attribs[i]));
        }
    }

    V(glLinkProgram(prog));
    // Detach shaders in an ideal world (no buggy drivers)
    //V(glDetachShader(prog, vso));
    //V(glDetachShader(prog, pso));

    GLint status;
    V(glGetProgramiv(prog, GL_LINK_STATUS, &status));
    if (status == 0)
    {
        GLint len = 0;
        V(glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len));
        
        if (len == 0)
        {
            V(glDeleteProgram(prog));
            NS_FATAL("Linking program (FRAGMENT: %d, VERTEX: %d)", vso, pso);
        }
        else
        {
            char error[512];
            V(glGetProgramInfoLog(prog, 512, 0, error));
            V(glDeleteProgram(prog));
            NS_FATAL("%s", error);
        }
    }

    return prog;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::BindTextureLocation(GLuint program, const char* name, GLint unit)
{
    GLint location = glGetUniformLocation(program, name);
    if (location != -1)
    {
        UseProgram(program);
        V(glUniform1i(location, unit));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::ActivateProgram(const Batch& batch)
{
    NS_ASSERT(batch.shader.v < NS_COUNTOF(mPrograms));
    ProgramInfo& programInfo = mPrograms[batch.shader.v];

    // Use program
    NS_ASSERT(programInfo.object != 0);
    UseProgram(programInfo.object);

    // Upload constants
    if (programInfo.projectionMtxHash != batch.projMtxHash)
    {
        V(glUniformMatrix4fv(programInfo.projectionMtxLocation, 1, GL_FALSE, *batch.projMtx));
        programInfo.projectionMtxHash = batch.projMtxHash;
    }

    if (programInfo.opacityLocation != -1 && programInfo.opacityHash != batch.opacityHash)
    {
        NS_ASSERT(batch.opacity != 0);
        V(glUniform1f(programInfo.opacityLocation, *batch.opacity));
        programInfo.opacityHash = batch.opacityHash;
    }

    if (programInfo.radialGradLocation != -1 && programInfo.radialGradHash != batch.radialGradHash)
    {
        NS_ASSERT(batch.radialGrad != 0);
        V(glUniform4fv(programInfo.radialGradLocation, 2, *batch.radialGrad));
        programInfo.radialGradHash = batch.radialGradHash;
    }

    if (programInfo.rgbaLocation != -1 && programInfo.rgbaHash != batch.rgbaHash)
    {
        NS_ASSERT(batch.rgba != 0);
        GLfloat v0 = (*batch.rgba)[0];
        GLfloat v1 = (*batch.rgba)[1];
        GLfloat v2 = (*batch.rgba)[2];
        GLfloat v3 = (*batch.rgba)[3];
        V(glUniform4f(programInfo.rgbaLocation, v0, v1, v2, v3));
        programInfo.rgbaHash = batch.rgbaHash;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::UseProgram(GLuint name)
{
    NS_ASSERT(glIsProgram(name));

    if (mProgram != name)
    {
        V(glUseProgram(name));
        mProgram = name;
    }

    NS_ASSERT(GetInteger(GL_CURRENT_PROGRAM) == GLint(name));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::DeleteShader(GLuint shader) const
{
    NS_ASSERT(glIsShader(shader));
    V(glDeleteShader(shader));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::DeleteProgram(GLuint program) const
{
    NS_ASSERT(glIsProgram(program));
    V(glDeleteProgram(program));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::SetRenderState(GLRenderState state)
{
    if (mRenderState.f.invalidated)
    {
        mRenderState.v = ~state.v;
    }

    GLRenderState delta;
    delta.v = mRenderState.v ^ state.v;

    if (delta.v != 0)
    {
        if (delta.f.scissorEnable != 0)
        {
            if (state.f.scissorEnable)
            {
                V(glEnable(GL_SCISSOR_TEST));
            }
            else
            {
                V(glDisable(GL_SCISSOR_TEST));
            }
        }

        if (delta.f.colorMask != 0)
        {
            GLboolean red = (state.f.colorMask & RedMask) > 0;
            GLboolean green = (state.f.colorMask & GreenMask) > 0;
            GLboolean blue = (state.f.colorMask & BlueMask) > 0;
            GLboolean alpha = (state.f.colorMask & AlphaMask) > 0;
            V(glColorMask(red, green, blue, alpha));
        }

        if (delta.f.blendEnable != 0)
        {
            if (state.f.blendEnable)
            {
                V(glEnable(GL_BLEND));
            }
            else
            {
                V(glDisable(GL_BLEND));
            }
        }

        if (delta.f.stencilEnable != 0)
        {
            if (state.f.stencilEnable)
            {
                V(glEnable(GL_STENCIL_TEST));
            }
            else
            {
                V(glDisable(GL_STENCIL_TEST));
            }
        }

        if (delta.f.stencilMode != 0)
        {
            switch ((StencilMode::Enum)state.f.stencilMode)
            {
                case StencilMode::Disabled:
                {
                    break;
                }
                case StencilMode::Equal_Keep:
                {
                    V(glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP));
                    break;
                }
                case StencilMode::Equal_Incr:
                {
                    V(glStencilOp(GL_KEEP, GL_KEEP, GL_INCR_WRAP));
                    break;
                }
                case StencilMode::Equal_Decr:
                {
                    V(glStencilOp(GL_KEEP, GL_KEEP, GL_DECR_WRAP));
                    break;
                }
                default:
                {
                    NS_ASSERT_UNREACHABLE;
                }
            }
        }

        if (delta.f.stencilRef != 0)
        {
            V(glStencilFunc(GL_EQUAL, state.f.stencilRef, 0xff));
        }

        mRenderState = state;
    }

    NS_ASSERT(glIsEnabled(GL_SCISSOR_TEST) == state.f.scissorEnable);
    NS_ASSERT(GetBoolean(GL_COLOR_WRITEMASK, 0) == ((state.f.colorMask & RedMask) > 0));
    NS_ASSERT(GetBoolean(GL_COLOR_WRITEMASK, 1) == ((state.f.colorMask & GreenMask) > 0));
    NS_ASSERT(GetBoolean(GL_COLOR_WRITEMASK, 2) == ((state.f.colorMask & BlueMask) > 0));
    NS_ASSERT(GetBoolean(GL_COLOR_WRITEMASK, 3) == ((state.f.colorMask & AlphaMask) > 0));
    NS_ASSERT(glIsEnabled(GL_BLEND) == state.f.blendEnable);
    NS_ASSERT(glIsEnabled(GL_STENCIL_TEST) == state.f.stencilEnable);
    NS_ASSERT(GetInteger(GL_STENCIL_FAIL) == GL_KEEP);
    NS_ASSERT(GetInteger(GL_STENCIL_PASS_DEPTH_FAIL) == GL_KEEP);
    NS_ASSERT(state.f.stencilMode != StencilMode::Equal_Keep || GetInteger(
        GL_STENCIL_PASS_DEPTH_PASS) == GL_KEEP);
    NS_ASSERT(state.f.stencilMode != StencilMode::Equal_Incr || GetInteger(
        GL_STENCIL_PASS_DEPTH_PASS) == GL_INCR_WRAP);
    NS_ASSERT(state.f.stencilMode != StencilMode::Equal_Decr || GetInteger(
        GL_STENCIL_PASS_DEPTH_PASS) == GL_DECR_WRAP);
    NS_ASSERT(GetInteger(GL_STENCIL_FUNC) == GL_EQUAL);
    NS_ASSERT(GetInteger(GL_STENCIL_REF) == (GLint)state.f.stencilRef);
    NS_ASSERT(GetInteger(GL_STENCIL_VALUE_MASK) == 0xff);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::EnableScissor()
{
    if (mRenderState.f.scissorEnable != 1)
    {
        mRenderState.f.scissorEnable = 1;
        V(glEnable(GL_SCISSOR_TEST));
    }

    NS_ASSERT(glIsEnabled(GL_SCISSOR_TEST));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::EnableColor()
{
    if (mRenderState.f.colorMask != 0xF)
    {
        mRenderState.f.colorMask = 0xF;
        V(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
    }

    NS_ASSERT(GetBoolean(GL_COLOR_WRITEMASK, 0) == GL_TRUE);
    NS_ASSERT(GetBoolean(GL_COLOR_WRITEMASK, 1) == GL_TRUE);
    NS_ASSERT(GetBoolean(GL_COLOR_WRITEMASK, 2) == GL_TRUE);
    NS_ASSERT(GetBoolean(GL_COLOR_WRITEMASK, 3) == GL_TRUE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::Scissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    if (mScissorX != x || mScissorY != y || mScissorWidth != width || mScissorHeight != height)
    {
        V(glScissor(x, y, width, height));
        mScissorX = x;
        mScissorY = y;
        mScissorWidth = width;
        mScissorHeight = height;
    }

    NS_ASSERT(GetInteger(GL_SCISSOR_BOX, 0) == GLint(x));
    NS_ASSERT(GetInteger(GL_SCISSOR_BOX, 1) == GLint(y));
    NS_ASSERT(GetInteger(GL_SCISSOR_BOX, 2) == GLint(width));
    NS_ASSERT(GetInteger(GL_SCISSOR_BOX, 3) == GLint(height));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::Viewport(uint32_t width, uint32_t height)
{
    if (mViewportWidth != width || mViewportHeight != height)
    {
        V(glViewport(0, 0, width, height));
        mViewportWidth = width;
        mViewportHeight = height;
    }

    NS_ASSERT(GetInteger(GL_VIEWPORT, 0) == (GLint)0);
    NS_ASSERT(GetInteger(GL_VIEWPORT, 1) == (GLint)0);
    NS_ASSERT(GetInteger(GL_VIEWPORT, 2) == (GLint)width);
    NS_ASSERT(GetInteger(GL_VIEWPORT, 3) == (GLint)height);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::SetClearColor(uint32_t color)
{
    if (mClearColor != color)
    {
        float r = ((color >> 16) & 0xff) / 255.0f;
        float g = ((color >> 8) & 0xff) / 255.0f;
        float b = ((color >> 0) & 0xff) / 255.0f;
        float a = ((color >> 24) & 0xff) / 255.0f;
        V(glClearColor(r, g, b, a));
        mClearColor = color;
    }

    NS_ASSERT(GetFloat(GL_COLOR_CLEAR_VALUE, 0) == ((color >> 16) & 0xff) / 255.0f);
    NS_ASSERT(GetFloat(GL_COLOR_CLEAR_VALUE, 1) == ((color >> 8) & 0xff) / 255.0f);
    NS_ASSERT(GetFloat(GL_COLOR_CLEAR_VALUE, 2) == ((color >> 0) & 0xff) / 255.0f);
    NS_ASSERT(GetFloat(GL_COLOR_CLEAR_VALUE, 3) == ((color >> 24) & 0xff) / 255.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::SetClearStencil(uint32_t stencil)
{
    if (mClearStencil != stencil)
    {
        V(glClearStencil(stencil));
        mClearStencil = stencil;
    }

    NS_ASSERT(GetInteger(GL_STENCIL_CLEAR_VALUE) == GLint(stencil));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLRenderDevice::GLRenderState::GLRenderState(RenderState state, GLint stencilRef)
{
    f.invalidated = 0;
    f.scissorEnable = state.f.scissorEnable;
    f.colorMask = state.f.colorEnable? 0xF: 0;
    f.blendEnable = state.f.blendMode == BlendMode::SrcOver;
    f.stencilEnable = state.f.stencilMode != StencilMode::Disabled;
    f.stencilMode = state.f.stencilMode;
    f.stencilRef = stencilRef;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::BindAPI()
{
#if NS_RENDERER_USE_WGL
    mOpenGL32 = LoadLibraryA("opengl32.dll");
    if (mOpenGL32 == 0)
    {
        NS_FATAL("Loading opengl32.dll");
    }

    wglGetProcAddress = (PFNWGLGETPROCADDRESSPROC)GetProcAddress(mOpenGL32, "wglGetProcAddress");
    wglGetCurrentContext = (PFNWGLGETCURRENTCONTEXT)GetProcAddress(mOpenGL32, "wglGetCurrentContext");

    #define GL_IMPORT(_optional, _proto, _func) \
    { \
        _func = (_proto)wglGetProcAddress(#_func); \
        if (_func == 0) \
        { \
            _func = (_proto)GetProcAddress(mOpenGL32, #_func); \
        } \
        if (_func == 0 && !_optional) \
        { \
            NS_FATAL("wglGetProcAddress %s", #_func); \
        } \
    }
    #include "GLImports.h"
    #undef GL_IMPORT

#elif NS_RENDERER_USE_EGL
    #define GL_IMPORT(_optional, _proto, _func) \
    { \
        _func = (_proto)eglGetProcAddress(#_func); \
        if (_func == 0 && !_optional) \
        { \
            NS_FATAL("eglGetProcAddress %s", #_func); \
        } \
    }
    #include "GLImports.h"
    #undef GL_IMPORT

#elif NS_RENDERER_USE_GLX
    #define GL_IMPORT(_optional, _proto, _func) \
    { \
        _func = (_proto)glXGetProcAddressARB((const GLubyte *)#_func); \
        if (_func == 0 && !_optional) \
        { \
            NS_FATAL("glXGetProcAddress %s", #_func); \
        } \
    }
    #include "GLImports.h"
    #undef GL_IMPORT

#elif NS_RENDERER_USE_NSGL
    const char* oglPath = "/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL";
    void* ogl = dlopen(oglPath, RTLD_NOLOAD);
    if (ogl == 0)
    {
        NS_FATAL("dlopen OpenGL");
    }

    #define GL_IMPORT(_optional, _proto, _func) \
    { \
        _func = (_proto)dlsym(ogl, #_func); \
        if (_func == 0 && !_optional) \
        { \
            NS_FATAL("dlsym %s", #_func); \
        } \
    }
    #include "GLImports.h"
    #undef GL_IMPORT

    dlclose(ogl);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::DetectGLVersion()
{
    uint32_t major;
    uint32_t minor;
    const char* version = (const char*)glGetString(GL_VERSION);
    NS_ASSERT(version != 0);

#if NS_RENDERER_OPENGL
    if (sscanf(version, "%d.%d", &major, &minor) != 2)
    {
        NS_DEBUG_BREAK;
        NS_LOG_WARNING("Unable to parse GL_VERSION '%s'", version);
    }

#else
    if (sscanf(version, "OpenGL ES %d.%d", &major, &minor) != 2)
    {
        char profile[2];
        if (sscanf(version, "OpenGL ES-%c%c %d.%d", profile, profile + 1, &major, &minor) != 4)
        {
            NS_DEBUG_BREAK;
            NS_LOG_WARNING("Unable to parse GL_VERSION '%s'", version);
        }
    }

#endif

    mGLVersion = 10 * major + minor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::FindExtensions()
{
#if NS_RENDERER_OPENGL
    #define GL_CORE(major, minor) NS_GL_VER(major, minor)
    #define GL_ES_CORE(major, minor) 0
#else
    #define GL_CORE(major, minor) 0
    #define GL_ES_CORE(major, minor) NS_GL_VER(major, minor)
#endif

    DetectGLVersion();

    struct Extension_
    {
        const char* name;
        uint32_t coreInVersion;
    };

    const Extension_ Extensions[Extension::Count] =
    {
        { "GL_ANGLE_framebuffer_blit",              GL_CORE(0,0) + GL_ES_CORE(0,0) },
        { "GL_ANGLE_framebuffer_multisample",       GL_CORE(0,0) + GL_ES_CORE(0,0) },
        { "GL_EXT_map_buffer_range",                GL_CORE(0,0) + GL_ES_CORE(0,0) },
        { "GL_EXT_discard_framebuffer",             GL_CORE(0,0) + GL_ES_CORE(0,0) },
        { "GL_EXT_framebuffer_blit",                GL_CORE(0,0) + GL_ES_CORE(0,0) },
        { "GL_EXT_framebuffer_multisample",         GL_CORE(0,0) + GL_ES_CORE(0,0) },
        { "GL_EXT_multisampled_render_to_texture",  GL_CORE(0,0) + GL_ES_CORE(0,0) },
        { "GL_EXT_debug_marker",                    GL_CORE(0,0) + GL_ES_CORE(0,0) },
        { "GL_EXT_texture_storage",                 GL_CORE(0,0) + GL_ES_CORE(0,0) },
        { "GL_EXT_texture_rg",                      GL_CORE(0,0) + GL_ES_CORE(0,0) },
        { "GL_EXT_texture_border_clamp",            GL_CORE(0,0) + GL_ES_CORE(0,0) },
        { "GL_APPLE_framebuffer_multisample",       GL_CORE(0,0) + GL_ES_CORE(0,0) },
        { "GL_APPLE_texture_max_level",             GL_CORE(2,1) + GL_ES_CORE(3,0) },
        { "GL_APPLE_vertex_array_object",           GL_CORE(0,0) + GL_ES_CORE(0,0) },
        { "GL_ARB_debug_output",                    GL_CORE(0,0) + GL_ES_CORE(0,0) },
        { "GL_ARB_map_buffer_range",                GL_CORE(3,0) + GL_ES_CORE(3,0) },
        { "GL_ARB_vertex_array_object",             GL_CORE(3,0) + GL_ES_CORE(3,0) },
        { "GL_ARB_framebuffer_object",              GL_CORE(3,0) + GL_ES_CORE(3,0) },
        { "GL_ARB_invalidate_subdata",              GL_CORE(4,3) + GL_ES_CORE(3,0) },
        { "GL_ARB_ES3_compatibility",               GL_CORE(4,3) + GL_ES_CORE(3,0) },
        { "GL_ARB_texture_storage",                 GL_CORE(4,2) + GL_ES_CORE(3,0) },
        { "GL_ARB_texture_rg",                      GL_CORE(3,0) + GL_ES_CORE(3,0) },
        { "GL_ARB_vertex_attrib_binding",           GL_CORE(4,3) + GL_ES_CORE(3,1) },
        { "GL_ARB_sampler_objects",                 GL_CORE(3,3) + GL_ES_CORE(3,0) },
        { "GL_ARB_texture_border_clamp",            GL_CORE(1,3) + GL_ES_CORE(3,2) },
        { "GL_OES_rgb8_rgba8",                      GL_CORE(3,0) + GL_ES_CORE(3,0) },
        { "GL_OES_vertex_array_object",             GL_CORE(0,0) + GL_ES_CORE(0,0) },
        { "GL_IMG_multisampled_render_to_texture",  GL_CORE(0,0) + GL_ES_CORE(0,0) },
        { "GL_KHR_debug",                           GL_CORE(4,3) + GL_ES_CORE(3,2) }
    };

    memset(mExtensions, 0, sizeof(mExtensions));

    {
        NS_LOG_TRACE("------------------GL extensions------------------");

        if (mGLVersion >= NS_GL_VER(3,0))
        {
            GLint numExtensions = 0;
            V(glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions));

            for (GLint i = 0; i < numExtensions; i++)
            {
                const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);

                for (uint32_t j = 0; j < Extension::Count; j++)
                {
                    if (String::Equals(extension, Extensions[j].name))
                    {
                        mExtensions[j].supported = true;
                        break;
                    }
                }
            }
        }
        else
        {
            const char* extensions = (const char*)glGetString(GL_EXTENSIONS);

            if (extensions != 0)
            {
                for (uint32_t i = 0; i < Extension::Count; i++)
                {
                    if (String::FindFirst(extensions, Extensions[i].name) != -1)
                    {
                        mExtensions[i].supported = true;
                    }
                }
            }
        }

        for (uint32_t i = 0; i < Extension::Count; i++)
        {
            // Mark ARB extensions already in Core as supported
            uint32_t coreInVersion = Extensions[i].coreInVersion;
            if (!mExtensions[i].supported && coreInVersion != 0 && mGLVersion >= coreInVersion)
            {
                mExtensions[i].supported = true;
                NS_LOG_TRACE("[+] %s", Extensions[i].name);
            }
            else
            {
                NS_LOG_TRACE("[%c] %s", mExtensions[i].supported ? '*' : ' ', Extensions[i].name);
            }
        }
    }

    MapBufferRange_ = 0;
    UnmapBuffer_ = 0;
    BindVertexArray_ = 0;
    DeleteVertexArrays_ = 0;
    GenVertexArrays_ = 0;
    IsVertexArray_ = 0;
    BlitFramebuffer_ = 0;
    RenderbufferStorageMultisample_ = 0;
    FramebufferTexture2DMultisample_ = 0;
    InvalidateFramebuffer_ = 0;
    PushGroupMarker_ = 0;
    PopGroupMarker_ = 0;
    TexStorage2D_ = 0;

    ////////////////////
    // MapBufferRange //
    ////////////////////
#if !NS_RENDERER_USE_EMS
    if (IsSupported(Extension::ARB_map_buffer_range))
    {
        MapBufferRange_ = glMapBufferRange;
        UnmapBuffer_ = glUnmapBuffer;
    }
    else if (IsSupported(Extension::EXT_map_buffer_range))
    {
#if GL_EXT_map_buffer_range
        MapBufferRange_ = glMapBufferRangeEXT;
        UnmapBuffer_ = glUnmapBufferOES;
#else
        NS_ASSERT_UNREACHABLE;
#endif
    }
#endif

    ///////////////////////
    // VertexArrayObject //
    ///////////////////////
    if (IsSupported(Extension::ARB_vertex_array_object))
    {
        BindVertexArray_ = glBindVertexArray;
        DeleteVertexArrays_ = glDeleteVertexArrays;
        GenVertexArrays_ = glGenVertexArrays;
        IsVertexArray_ = glIsVertexArray;
    }
    else if (IsSupported(Extension::APPLE_vertex_array_object))
    {
#if GL_APPLE_vertex_array_object
        BindVertexArray_ = glBindVertexArrayAPPLE;
        DeleteVertexArrays_ = glDeleteVertexArraysAPPLE;
        GenVertexArrays_ = glGenVertexArraysAPPLE;
        IsVertexArray_ = glIsVertexArrayAPPLE;
#else
        NS_ASSERT_UNREACHABLE;
#endif
    }
    else if (IsSupported(Extension::OES_vertex_array_object))
    {
#if GL_OES_vertex_array_object
        BindVertexArray_ = glBindVertexArrayOES;
        DeleteVertexArrays_ = glDeleteVertexArraysOES;
        GenVertexArrays_ = glGenVertexArraysOES;
        IsVertexArray_ = glIsVertexArrayOES;
#else
        NS_ASSERT_UNREACHABLE;
#endif
    }

    /////////////////////
    // BlitFramebuffer //
    /////////////////////
    if (IsSupported(Extension::ARB_framebuffer_object))
    {
        BlitFramebuffer_ = glBlitFramebuffer;
    }
    else if (IsSupported(Extension::EXT_framebuffer_blit))
    {
#if GL_EXT_framebuffer_blit
        BlitFramebuffer_ = glBlitFramebufferEXT;
#else
        NS_ASSERT_UNREACHABLE;
#endif
    }
    else if (IsSupported(Extension::ANGLE_framebuffer_blit))
    {
#if GL_ANGLE_framebuffer_blit
        BlitFramebuffer_ = glBlitFramebufferANGLE;
#else
        NS_ASSERT_UNREACHABLE;
#endif
    }

    ////////////////////////////////////
    // RenderbufferStorageMultisample //
    ////////////////////////////////////
    if (IsSupported(Extension::EXT_multisampled_render_to_texture))
    {
#if GL_EXT_multisampled_render_to_texture
        RenderbufferStorageMultisample_ = glRenderbufferStorageMultisampleEXT;
#else
        NS_ASSERT_UNREACHABLE;
#endif
    }
    else if (IsSupported(Extension::IMG_multisampled_render_to_texture))
    {
#if GL_IMG_multisampled_render_to_texture
        RenderbufferStorageMultisample_ = glRenderbufferStorageMultisampleIMG;
#else
        NS_ASSERT_UNREACHABLE;
#endif
    }
    else if (IsSupported(Extension::ARB_framebuffer_object))
    {
        RenderbufferStorageMultisample_ = glRenderbufferStorageMultisample;
    }
    else if (IsSupported(Extension::EXT_framebuffer_multisample))
    {
#if GL_EXT_framebuffer_multisample
        RenderbufferStorageMultisample_ = glRenderbufferStorageMultisampleEXT;
#else
        NS_ASSERT_UNREACHABLE;
#endif
    }
    else if (IsSupported(Extension::APPLE_framebuffer_multisample))
    {
#if GL_APPLE_framebuffer_multisample
        RenderbufferStorageMultisample_ = glRenderbufferStorageMultisampleAPPLE;
#else
        NS_ASSERT_UNREACHABLE;
#endif
    }
    else if (IsSupported(Extension::ANGLE_framebuffer_multisample))
    {
#if GL_ANGLE_framebuffer_multisample
        RenderbufferStorageMultisample_ = glRenderbufferStorageMultisampleANGLE;
#else
        NS_ASSERT_UNREACHABLE;
#endif
    }

    /////////////////////////////////////
    // FramebufferTexture2DMultisample //
    /////////////////////////////////////
    if (IsSupported(Extension::EXT_multisampled_render_to_texture))
    {
#if GL_EXT_multisampled_render_to_texture
        FramebufferTexture2DMultisample_ = glFramebufferTexture2DMultisampleEXT;
#else
        NS_ASSERT_UNREACHABLE;
#endif
    }
    else if (IsSupported(Extension::IMG_multisampled_render_to_texture))
    {
#if GL_IMG_multisampled_render_to_texture
        FramebufferTexture2DMultisample_ = glFramebufferTexture2DMultisampleIMG;
#else
        NS_ASSERT_UNREACHABLE;
#endif
    }

    ///////////////////////////
    // InvalidateFramebuffer //
    ///////////////////////////
    if (IsSupported(Extension::ARB_invalidate_subdata))
    {
#if GL_VERSION_4_3 || GL_ES_VERSION_3_0 || ARB_invalidate_subdata
        InvalidateFramebuffer_ = glInvalidateFramebuffer;
#else
        NS_ASSERT_UNREACHABLE;
#endif
    }
    else if (IsSupported(Extension::EXT_discard_framebuffer))
    {
#if GL_EXT_discard_framebuffer
        InvalidateFramebuffer_ = glDiscardFramebufferEXT;
#else
        NS_ASSERT_UNREACHABLE;
#endif
    }

    /////////////////////
    // PushGroupMarker //
    /////////////////////
    if (IsSupported(Extension::EXT_debug_marker))
    {
#if GL_EXT_debug_marker
        PushGroupMarker_ = glPushGroupMarkerEXT;
        PopGroupMarker_ = glPopGroupMarkerEXT;
#else
        NS_ASSERT_UNREACHABLE;
#endif
    }

    ////////////////
    // TexStorage //
    ////////////////
    if (IsSupported(Extension::ARB_texture_storage))
    {
        TexStorage2D_ = glTexStorage2D;
    }
    else if (IsSupported(Extension::EXT_texture_storage))
    {
#if GL_EXT_texture_storage
        TexStorage2D_ = glTexStorage2DEXT;
#else
        NS_ASSERT_UNREACHABLE;
#endif
    }

#if !NS_RENDERER_USE_EMS
    // Check that TexStorage is not bugged (AMD Catalyst 13.90)
    if (TexStorage2D_ != 0)
    {
        GLuint name;
        V(glGenTextures(1, &name));
        V(glBindTexture(GL_TEXTURE_2D, name));

        uint8_t data;
        TexStorage2D_(GL_TEXTURE_2D, 1, GL_RGBA8, 2, 2);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data);

        if (glGetError() != GL_NO_ERROR)
        {
            mExtensions[Extension::ARB_texture_storage].supported = false;
            TexStorage2D_ = 0;
            while (glGetError() != GL_NO_ERROR);
        }

        V(glDeleteTextures(1, &name));
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool GLRenderDevice::IsSupported(Extension::Enum extension) const
{
    return mExtensions[extension].supported;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t GLRenderDevice::MaxSamples() const
{
    if (IsSupported(Extension::EXT_multisampled_render_to_texture))
    {
        return GetInteger(GL_MAX_SAMPLES_EXT);
    }
    else if (IsSupported(Extension::IMG_multisampled_render_to_texture))
    {
        return GetInteger(GL_MAX_SAMPLES_IMG);
    }
    else if (IsSupported(Extension::ARB_framebuffer_object))
    {
        return GetInteger(GL_MAX_SAMPLES);
    }
    else if (IsSupported(Extension::EXT_framebuffer_multisample))
    {
        return GetInteger(GL_MAX_SAMPLES_EXT);
    }
    else if (IsSupported(Extension::ANGLE_framebuffer_multisample))
    {
        return GetInteger(GL_MAX_SAMPLES_ANGLE);
    }
    else if (IsSupported(Extension::APPLE_framebuffer_multisample))
    {
        return GetInteger(GL_MAX_SAMPLES_APPLE);
    }

    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::FillCaps()
{
    mCaps = {};

    mCaps.centerPixelOffset = 0.0f;
    mCaps.dynamicVerticesSize = DYNAMIC_VB_SIZE;
    mCaps.dynamicIndicesSize = DYNAMIC_IB_SIZE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::PushGroupMarker(GLsizei length, const char *marker) const
{
    if (PushGroupMarker_ != 0)
    {
        V(PushGroupMarker_(length, marker));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GLRenderDevice::PopGroupMarker() const
{
    if (PopGroupMarker_ != 0)
    {
        V(PopGroupMarker_());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLint GLRenderDevice::GetInteger(GLenum name) const
{
    GLint vv;
    V(glGetIntegerv(name, &vv));
    return vv;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLint GLRenderDevice::GetInteger(GLenum name, uint32_t i) const
{
    NS_ASSERT(i < 4);
    GLint vv[4];
    V(glGetIntegerv(name, vv));
    return vv[i];
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLfloat GLRenderDevice::GetFloat(GLenum name) const
{
    GLfloat vv;
    V(glGetFloatv(name, &vv));
    return vv;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLfloat GLRenderDevice::GetFloat(GLenum name, uint32_t i) const
{
    NS_ASSERT(i < 4);
    GLfloat vv[4];
    V(glGetFloatv(name, vv));
    return vv[i];
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLboolean GLRenderDevice::GetBoolean(GLenum name, uint32_t i) const
{
    NS_ASSERT(i < 4);
    GLboolean vv[4];
    V(glGetBooleanv(name, vv));
    return vv[i];
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLboolean GLRenderDevice::VertexAttribArrayEnabled(GLuint index) const
{
    GLint vv;
    V(glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &vv));
    return vv > 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
GLint GLRenderDevice::GetTexParameter(GLenum name) const
{
    GLint vv;
    V(glGetTexParameteriv(GL_TEXTURE_2D, name, &vv));
    return vv;
}
