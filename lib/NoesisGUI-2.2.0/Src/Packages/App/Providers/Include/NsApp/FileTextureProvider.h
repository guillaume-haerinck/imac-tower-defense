////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_FILETEXTUREPROVIDER_H__
#define __APP_FILETEXTUREPROVIDER_H__


#include <NsCore/Noesis.h>
#include <NsGui/TextureProvider.h>
#include <NsApp/ProvidersApi.h>


namespace Noesis
{

class Stream;

}

namespace NoesisApp
{

NS_WARNING_PUSH
NS_MSVC_WARNING_DISABLE(4251 4275)

////////////////////////////////////////////////////////////////////////////////////////////////////
/// A texture provider that loads standard image files (jpg, png, bmp, gif)
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_APP_PROVIDERS_API FileTextureProvider: public Noesis::TextureProvider
{
public:
    FileTextureProvider();

protected:
    virtual Noesis::Ptr<Noesis::Stream> OpenStream(const char* uri) const = 0;

    /// From TextureProvider
    //@{
    Noesis::TextureInfo GetTextureInfo(const char* uri) override;
    Noesis::Ptr<Noesis::Texture> LoadTexture(const char* uri, Noesis::RenderDevice* device) override;
    //@}

private:
    uint8_t mGammaTable[256];
};

NS_WARNING_POP

}

#endif
