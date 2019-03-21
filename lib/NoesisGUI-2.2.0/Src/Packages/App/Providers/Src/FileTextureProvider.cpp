////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsApp/FileTextureProvider.h>
#include <NsCore/Stream.h>
#include <NsCore/Error.h>
#include <NsCore/Log.h>
#include <NsCore/Ptr.h>
#include <NsMath/Utils.h>
#include <NsRender/RenderDevice.h>
#include <NsRender/Texture.h>
#include <EASTL/algorithm.h>


NS_WARNING_PUSH

#ifdef _PREFAST_
#include <codeanalysis/warnings.h>
NS_MSVC_WARNING_DISABLE(ALL_CODE_ANALYSIS_WARNINGS)
#endif

NS_MSVC_WARNING_DISABLE(4244 4242 4100)
NS_GCC_CLANG_WARNING_DISABLE("-Wconversion")
NS_GCC_CLANG_WARNING_DISABLE("-Wunused-function")
#if NS_CLANG_HAS_WARNING("-Wcomma")
NS_CLANG_WARNING_DISABLE("-Wcomma")
#endif

#define STBI_MALLOC(sz) Noesis::Alloc(sz)
#define STBI_REALLOC(p,sz) Noesis::Realloc(p, sz)
#define STBI_FREE(p) Noesis::Dealloc(p)
#define STBI_ASSERT(x) NS_ASSERT(x)
#define STBI_NO_STDIO
#define STBI_NO_LINEAR
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_ONLY_GIF
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

NS_WARNING_POP

// The compiler can only determine unreferenced functions after it finished parsing the compiled
// source file, so we cannot disable it only for the header include
NS_MSVC_WARNING_DISABLE(4505)


using namespace Noesis;
using namespace NoesisApp;


namespace
{

////////////////////////////////////////////////////////////////////////////////////////////////////
int Read(void *user, char *data, int size)
{
    Stream* stream = (Stream*)user;
    return stream->Read(data, size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Skip(void *user, int n)
{
    Stream* stream = (Stream*)user;
    stream->SetPosition((int)stream->GetPosition() + n);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int Eof(void *user)
{
    Stream* stream = (Stream*)user;
    return stream->GetPosition() >= stream->GetLength();
}

}

////////////////////////////////////////////////////////////////////////////////////////////////////
FileTextureProvider::FileTextureProvider()
{
    for (uint32_t i = 0; i < 256; i++)
    {
        // https://en.wikipedia.org/wiki/SRGB
        float l = i / 255.0f;
        float v = l <= 0.0031308f ? l * 12.92f : 1.055f * powf(l, 1.0f / 2.4f) - 0.055f;
        mGammaTable[i] = (uint8_t)Clip(Round(v * 255.0f), 0, 255);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
TextureInfo FileTextureProvider::GetTextureInfo(const char* filename)
{
    TextureInfo info = { 0, 0 };

    Ptr<Stream> file = OpenStream(filename);
    if (file != 0)
    {
        int x, y, n;
        stbi_io_callbacks callbacks = { Read, Skip, Eof };
        if (stbi_info_from_callbacks(&callbacks, file, &x, &y, &n))
        {
            info.width = x;
            info.height = y;
        }
        else
        {
            NS_LOG_WARNING("%s: %s", filename, stbi_failure_reason());
        }
    }

    return info;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<Texture> FileTextureProvider::LoadTexture(const char* filename, RenderDevice* device)
{
    Ptr<Stream> file = OpenStream(filename);
    if (file == 0)
    {
        return nullptr;
    }

    int x, y, n;
    stbi_io_callbacks callbacks = { Read, Skip, Eof };
    stbi_uc* data = stbi_load_from_callbacks(&callbacks, file, &x, &y, &n, 4);
    if (data == 0)
    {
        NS_LOG_WARNING("%s: %s", filename, stbi_failure_reason());
        return nullptr;
    }

    // Premultiply alpha
    if (n == 4)
    {
        bool sRGB = device->GetCaps().linearRendering;
        for (int i = 0; i < x * y; i++)
        {
            // http://ssp.impulsetrain.com/gamma-premult.html
            stbi_uc a = sRGB ? mGammaTable[data[4 * i + 3]] : data[4 * i + 3];
            stbi_uc r = (stbi_uc)(((uint32_t)data[4 * i] * a) / 255);
            stbi_uc g = (stbi_uc)(((uint32_t)data[4 * i + 1] * a) / 255);
            stbi_uc b = (stbi_uc)(((uint32_t)data[4 * i + 2] * a) / 255);

            data[4 * i] = r;
            data[4 * i + 1] = g;
            data[4 * i + 2] = b;
        }
    }

    Ptr<Texture> texture = device->CreateTexture(filename, x, y, 1, TextureFormat::RGBA8);
    device->UpdateTexture(texture, 0, 0, 0, x, y, data);
    stbi_image_free(data);

    return texture;
}
