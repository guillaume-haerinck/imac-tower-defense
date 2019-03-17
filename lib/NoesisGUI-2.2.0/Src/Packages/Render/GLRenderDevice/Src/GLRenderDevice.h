////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __RENDER_GLRENDERDEVICE_H__
#define __RENDER_GLRENDERDEVICE_H__


#include <NsRender/RenderDevice.h>

#include "GLHeaders.h"


class GLTexture;
class GLRenderTarget;


namespace NoesisApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
/// GLRenderDevice
////////////////////////////////////////////////////////////////////////////////////////////////////
class GLRenderDevice final: public Noesis::RenderDevice
{
public:
    GLRenderDevice();
    ~GLRenderDevice();

    // Creates a Noesis texture from a GL texture
    static Noesis::Ptr<Noesis::Texture> WrapTexture(GLuint object, uint32_t width, uint32_t height,
        uint32_t levels, bool isInverted);

private:
    /// From RenderDevice
    //@{
    const Noesis::DeviceCaps& GetCaps() const override;
    Noesis::Ptr<Noesis::RenderTarget> CreateRenderTarget(const char* label, uint32_t width,
        uint32_t height, uint32_t sampleCount) override;
    Noesis::Ptr<Noesis::RenderTarget> CloneRenderTarget(const char* label,
        Noesis::RenderTarget* surface) override;
    Noesis::Ptr<Noesis::Texture> CreateTexture(const char* label, uint32_t width, uint32_t height,
        uint32_t numLevels, Noesis::TextureFormat::Enum format) override;
    void UpdateTexture(Noesis::Texture* texture, uint32_t level, uint32_t x, uint32_t y,
        uint32_t width, uint32_t height, const void* data) override;
    void BeginRender(bool offscreen) override;
    void SetRenderTarget(Noesis::RenderTarget* surface) override;
    void BeginTile(const Noesis::Tile& tile, uint32_t surfaceWidth,
        uint32_t surfaceHeight) override;
    void EndTile() override;
    void ResolveRenderTarget(Noesis::RenderTarget* surface, const Noesis::Tile* tiles,
        uint32_t numTiles) override;
    void EndRender() override;
    void* MapVertices(uint32_t bytes) override;
    void UnmapVertices() override;
    void* MapIndices(uint32_t bytes) override;
    void UnmapIndices() override;
    void DrawBatch(const Noesis::Batch& batch) override;
    //@}

    friend class ::GLTexture;
    friend class ::GLRenderTarget;
    void DeleteTexture(GLTexture* texture);
    void DeleteRenderTarget(GLRenderTarget* surface);

    void InvalidateStateCache();
    void UnbindObjects();

    struct DynamicBuffer;
    void CreateBuffer(DynamicBuffer& buffer, GLenum target, uint32_t size) const;
    void DestroyBuffer(DynamicBuffer& buffer) const;
    void BindBuffer(DynamicBuffer& buffer);
    void SetVertexFormat(uint8_t format, uintptr_t offset);
    void DisableClientState() const;
    void EnableVertexAttribArrays(uint32_t state);
    void* MapBuffer(DynamicBuffer& buffer, uint32_t size);
    void UnmapBuffer(DynamicBuffer& buffer);
    void CreateSamplers();
    void CreatePrograms();
    void CreateVertexFormats();
    GLboolean HaveMapBufferRange() const;
    GLvoid* MapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length,
        GLbitfield access) const;
    void UnmapBuffer(GLenum target) const;
    GLboolean HaveVertexArrayObject() const;
    void GenVertexArrays(GLsizei n, GLuint *arrays) const;
    void BindVertexArray(GLuint name);
    void DeleteVertexArrays(GLsizei n, const GLuint *arrays) const;
    GLboolean IsVertexArray(GLuint array) const;

    GLuint RenderbufferStorage(GLsizei samples, GLenum format, GLsizei width, GLsizei height) const;
    void RenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat,
        GLsizei width, GLsizei height) const;
    GLboolean HaveMultisampledRenderToTexture() const;
    void FramebufferTexture2DMultisample(GLenum target, GLenum attachment, GLenum textarget,
        GLuint texture, GLint level, GLsizei samples) const;
    GLuint CreateStencil(GLsizei width, GLsizei height, GLsizei samples) const;
    GLuint CreateColor(GLsizei width, GLsizei height, GLsizei samples) const;
    void CreateFBO(GLsizei samples, GLuint texture, GLuint stencil, GLuint colorAA, GLuint& fbo,
        GLuint& fboResolved) const;
    void Resolve(GLRenderTarget* target, const Noesis::Tile* tiles, uint32_t numTiles);
    void BindRenderTarget(GLRenderTarget* renderTarget);
    void DiscardFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments) const;
    bool HaveBlitFramebuffer() const;
    void BlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0,
        GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) const;
    void DeleteFramebuffer(GLuint framebuffer) const;
    void DeleteRenderbuffer(GLuint renderbuffer) const;

    bool HaveBorderClamp() const;
    bool HaveTexStorage() const;
    bool HaveTexRG() const;
    void GetGLFormat(Noesis::TextureFormat::Enum texFormat, GLenum& internalFormat, GLenum& format);
    GLint GLWrapSMode(Noesis::SamplerState sampler) const;
    GLint GLWrapTMode(Noesis::SamplerState sampler) const;
    GLint GLMinFilter(Noesis::SamplerState sampler) const;
    GLint GLMagFilter(Noesis::SamplerState sampler) const;

    void TexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width,
        GLsizei height);
    void InitTexture(GLTexture* texture, GLenum internalFormat, GLenum format);
    void SetTextureState(GLTexture* texture, Noesis::SamplerState state);
    void SetTexture(uint32_t unit, GLTexture* texture, Noesis::SamplerState state);
    void SetTextures(const Noesis::Batch& batch);
    void ActiveTexture(uint32_t unit);
    void BindTexture2D(GLuint texture);
    void SetUnpackState();
    void TexSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLint width, GLint height,
        GLenum format, const GLvoid* data);
    void BindSampler(GLuint sampler);
    void DeleteSampler(GLuint sampler) const;
    void DeleteTexture(GLuint texture) const;

    GLuint CompileShader(GLenum type, const char* source) const;
    GLuint LinkProgram(GLuint vso, GLuint pso, const char** binds) const;
    void BindTextureLocation(GLuint program, const char* name, GLint unit);
    void ActivateProgram(const Noesis::Batch& batch);
    void UseProgram(GLuint name);
    void DeleteShader(GLuint shader) const;
    void DeleteProgram(GLuint program) const;

    union GLRenderState;
    void SetRenderState(GLRenderState state);
    void EnableScissor();
    void EnableColor();
    void Scissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    void Viewport(uint32_t width, uint32_t height);
    void SetClearColor(uint32_t color);
    void SetClearStencil(uint32_t stencil);

    void BindAPI();
    void DetectGLVersion();
    void FindExtensions();
    uint32_t MaxSamples() const;
    void FillCaps();
 
    void PushGroupMarker(GLsizei length, const char *marker) const;
    void PopGroupMarker() const;

    GLint GetInteger(GLenum name) const;
    GLint GetInteger(GLenum name, uint32_t i) const;
    GLfloat GetFloat(GLenum name) const;
    GLfloat GetFloat(GLenum name, uint32_t i) const;
    GLboolean GetBoolean(GLenum name, uint32_t i) const;
    GLboolean VertexAttribArrayEnabled(GLuint index) const;
    GLint GetTexParameter(GLenum name) const;

private:
    uint32_t mGLVersion;

#if NS_RENDERER_USE_WGL
    HMODULE mOpenGL32;
    PFNWGLGETPROCADDRESSPROC wglGetProcAddress;
    PFNWGLGETCURRENTCONTEXT wglGetCurrentContext;
#endif

    #define GL_IMPORT(_optional, _proto, _func) _proto _func
    #include "GLImports.h"
    #undef GL_IMPORT

    struct Attr
    {
        enum
        {
            Pos,
            Color,
            Tex0,
            Tex1,
            Coverage,

            Count
        };
    };

    uint32_t mMaxVertexAttribs;

    struct VertexFormat
    {
        GLuint vao;
        GLsizei stride;
    };

    VertexFormat mVertexFormats[16];

    struct DynamicBuffer
    {
        GLenum target;
        GLuint objects[6];
        GLuint boundObject;
        uint32_t size;
        uint32_t pos;
        uint32_t drawPos;
        uint32_t index;
        void* cpuMemory;
    };

    DynamicBuffer mDynamicVB;
    DynamicBuffer mDynamicIB;
    GLuint mDefaultVAO;

    Noesis::DeviceCaps mCaps;

    struct ProgramInfo
    {
        uint8_t vertexFormat;
        GLuint object;

        GLint projectionMtxLocation;
        GLint opacityLocation;
        GLint radialGradLocation;
        GLint rgbaLocation;

        uint32_t projectionMtxHash;
        uint32_t opacityHash;
        uint32_t radialGradHash;
        uint32_t rgbaHash;
    };

    ProgramInfo mPrograms[84];
    GLuint mSamplers[64];

    struct TextureUnit
    {
        enum Enum
        {
            Pattern,
            Ramps,
            Image,
            Glyphs,

            Count
        };
    };

    union GLRenderState
    {
        struct
        {
            uint32_t invalidated:1;
            uint32_t scissorEnable:1;
            uint32_t colorMask:4;
            uint32_t blendEnable:1;
            uint32_t stencilEnable:1;
            uint32_t stencilMode:2;
            uint32_t stencilRef:8;

        } f;

        uint32_t v;

        GLRenderState(): v(0) {}
        GLRenderState(Noesis::RenderState state, GLint stencilRef);
    };

    //Cached state
    //@{
    uint32_t mViewportWidth, mViewportHeight;
    uint32_t mScissorX, mScissorY, mScissorWidth, mScissorHeight;
    uint64_t mClearColor;
    uint32_t mClearStencil;
    GLuint mVertexArray;
    uint32_t mActiveTextureUnit;
    GLRenderState mRenderState;
    GLuint mBoundTextures[TextureUnit::Count];
    GLuint mBoundSamplers[TextureUnit::Count];
    GLuint mProgram;
    uint32_t mEnabledVertexAttribArrays;
    GLRenderTarget* mBoundRenderTarget;
    bool mUnpackStateSet;
    //@}

    struct Extension
    {
        enum Enum
        {
            ANGLE_framebuffer_blit,
            ANGLE_framebuffer_multisample,
            EXT_map_buffer_range,
            EXT_discard_framebuffer,
            EXT_framebuffer_blit,
            EXT_framebuffer_multisample,
            EXT_multisampled_render_to_texture,
            EXT_debug_marker,
            EXT_texture_storage,
            EXT_texture_rg,
            EXT_texture_border_clamp,
            APPLE_framebuffer_multisample,
            APPLE_texture_max_level,
            APPLE_vertex_array_object,
            ARB_debug_output,
            ARB_map_buffer_range,
            ARB_vertex_array_object,
            ARB_framebuffer_object,
            ARB_invalidate_subdata,
            ARB_ES3_compatibility,
            ARB_texture_storage,
            ARB_texture_rg,
            ARB_vertex_attrib_binding,
            ARB_sampler_objects,
            ARB_texture_border_clamp,
            OES_rgb8_rgba8,
            OES_vertex_array_object,
            IMG_multisampled_render_to_texture,
            KHR_debug,

            Count
        };

        bool supported;
    };

    bool IsSupported(Extension::Enum extension) const;
    Extension mExtensions[Extension::Count];

    GLvoid* (GL_APIENTRYP MapBufferRange_)(GLenum, GLintptr, GLsizeiptr, GLbitfield);
    GLboolean (GL_APIENTRYP UnmapBuffer_)(GLenum);
    void (GL_APIENTRYP BindVertexArray_)(GLuint);
    void (GL_APIENTRYP DeleteVertexArrays_)(GLsizei, const GLuint *);
    void (GL_APIENTRYP GenVertexArrays_)(GLsizei, GLuint *);
    GLboolean (GL_APIENTRYP IsVertexArray_)(GLuint array);
    void (GL_APIENTRYP BlitFramebuffer_)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint,
        GLbitfield, GLenum);
    void (GL_APIENTRYP RenderbufferStorageMultisample_)(GLenum, GLsizei, GLenum, GLsizei, GLsizei);
    void (GL_APIENTRYP FramebufferTexture2DMultisample_)(GLenum, GLenum, GLenum, GLuint, GLint,
        GLsizei);
    void (GL_APIENTRYP InvalidateFramebuffer_)(GLenum, GLsizei, const GLenum *);
    void (GL_APIENTRYP PushGroupMarker_)(GLsizei, const GLchar*);
    void (GL_APIENTRYP PopGroupMarker_)();
    void (GL_APIENTRYP TexStorage2D_)(GLenum, GLsizei, GLenum, GLsizei, GLsizei);
};

}

#endif
