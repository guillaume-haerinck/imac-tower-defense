////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsApp/EmbeddedFontProvider.h>
#include <NsCore/Stream.h>
#include <NsCore/StringUtils.h>
#include <NsCore/MemoryStream.h>


using namespace Noesis;
using namespace NoesisApp;


////////////////////////////////////////////////////////////////////////////////////////////////////
EmbeddedFontProvider::EmbeddedFontProvider(const EmbeddedFont* fonts, uint32_t size,
    FontProvider* fallback): mFallback(fallback)
{
    mFonts.assign(fonts, fonts + size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
FontSource EmbeddedFontProvider::MatchFont(const char* baseUri, const char* familyName,
    FontWeight weight, FontStretch stretch, FontStyle style)
{
    FontSource match = CachedFontProvider::MatchFont(baseUri, familyName, weight, stretch, style);
    if (match.file == 0 && mFallback != 0)
    {
        match = mFallback->MatchFont(baseUri, familyName, weight, stretch, style);
    }

    return match;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool EmbeddedFontProvider::FamilyExists(const char* baseUri, const char* familyName)
{
    bool exists = CachedFontProvider::FamilyExists(baseUri, familyName);
    if (!exists && mFallback != 0)
    {
        exists = mFallback->FamilyExists(baseUri, familyName);
    }

    return exists;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void EmbeddedFontProvider::ScanFolder(const char* folder)
{
    char id[8];

    for (uint32_t i = 0; i < mFonts.size(); i++)
    {
        if (String::Equals(folder, mFonts[i].folder))
        {
            snprintf(id, sizeof(id), "%d", i);
            RegisterFont(folder, id);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<Stream> EmbeddedFontProvider::OpenFont(const char* folder, const char* filename) const
{
    char id[8];

    for (uint32_t i = 0; i < mFonts.size(); i++)
    {
        if (String::Equals(folder, mFonts[i].folder))
        {
            snprintf(id, sizeof(id), "%d", i);
            if (String::Equals(filename, id))
            {
                return *new MemoryStream(mFonts[i].data, mFonts[i].size);
            }
        }
    }

    return nullptr;
}
