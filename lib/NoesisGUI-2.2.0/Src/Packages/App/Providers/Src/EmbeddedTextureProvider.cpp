////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsApp/EmbeddedTextureProvider.h>
#include <NsCore/Stream.h>
#include <NsCore/StringUtils.h>
#include <NsCore/MemoryStream.h>
#include <NsRender/Texture.h>


using namespace Noesis;
using namespace NoesisApp;


////////////////////////////////////////////////////////////////////////////////////////////////////
EmbeddedTextureProvider::EmbeddedTextureProvider(const EmbeddedTexture* textures, uint32_t size,
    TextureProvider* fallback): mFallback(fallback)
{
    mTextures.assign(textures, textures + size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
TextureInfo EmbeddedTextureProvider::GetTextureInfo(const char* uri)
{
    TextureInfo info = FileTextureProvider::GetTextureInfo(uri);
    if (info.width == 0 && info.height == 0 && mFallback != 0)
    {
        info = mFallback->GetTextureInfo(uri);
    }

    return info;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<Texture> EmbeddedTextureProvider::LoadTexture(const char* uri, RenderDevice* device)
{
    Ptr<Texture> texture = FileTextureProvider::LoadTexture(uri, device);
    if (texture == 0 && mFallback != 0)
    {
        texture = mFallback->LoadTexture(uri, device);
    }

    return texture;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<Stream> EmbeddedTextureProvider::OpenStream(const char* uri) const
{
    for (const auto& texture: mTextures)
    {
        if (String::Equals(uri, texture.name))
        {
            return *new MemoryStream(texture.data, texture.size);
        }
    }

    return nullptr;
}
