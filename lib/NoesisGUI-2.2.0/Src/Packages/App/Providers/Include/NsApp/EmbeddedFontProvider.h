////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_EMBEDDEDFONTPROVIDER_H__
#define __APP_EMBEDDEDFONTPROVIDER_H__


#include <NsCore/Noesis.h>
#include <NsCore/Vector.h>
#include <NsCore/Ptr.h>
#include <NsApp/ProvidersApi.h>
#include <NsGui/CachedFontProvider.h>


namespace NoesisApp
{

struct EmbeddedFont
{
    const char* folder;
    const uint8_t* data;
    uint32_t size;
};

NS_WARNING_PUSH
NS_MSVC_WARNING_DISABLE(4251 4275)

////////////////////////////////////////////////////////////////////////////////////////////////////
///  A provider for fonts embeded in memory
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_APP_PROVIDERS_API EmbeddedFontProvider: public Noesis::CachedFontProvider
{
public:
    EmbeddedFontProvider(const EmbeddedFont* fonts, uint32_t size, FontProvider* fallback = 0);

private:
    /// From FontProvider
    //@{
    Noesis::FontSource MatchFont(const char* baseUri, const char* familyName,
        Noesis::FontWeight weight, Noesis::FontStretch stretch, Noesis::FontStyle style) override;
    bool FamilyExists(const char* baseUri, const char* familyName) override;
    //@}

    /// From CachedFontProvider
    //@{
    void ScanFolder(const char* folder) override;
    Noesis::Ptr<Noesis::Stream> OpenFont(const char* folder, const char* filename) const override;
    //@}

private:
    NsVector<EmbeddedFont> mFonts;
    Noesis::Ptr<FontProvider> mFallback;
};

NS_WARNING_POP

}

#endif
