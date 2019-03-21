////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_LOCALTEXTUREPROVIDER_H__
#define __APP_LOCALTEXTUREPROVIDER_H__


#include <NsCore/Noesis.h>
#include <NsApp/FileTextureProvider.h>


namespace NoesisApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
/// A texture provider that searches textures in local directories
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_APP_PROVIDERS_API LocalTextureProvider: public FileTextureProvider
{
public:
    LocalTextureProvider(const char* rootPath);

private:
    /// From FileTextureProvider
    //@{
    Noesis::Ptr<Noesis::Stream> OpenStream(const char* uri) const override;
    //@}

private:
    char mRootPath[512];
};

}

#endif
