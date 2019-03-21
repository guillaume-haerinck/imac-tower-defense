////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsApp/EmbeddedXamlProvider.h>
#include <NsCore/Stream.h>
#include <NsCore/StringUtils.h>
#include <NsCore/MemoryStream.h>


using namespace Noesis;
using namespace NoesisApp;


////////////////////////////////////////////////////////////////////////////////////////////////////
EmbeddedXamlProvider::EmbeddedXamlProvider(const EmbeddedXaml* xamls, uint32_t size,
    XamlProvider* fallback): mFallback(fallback)
{
    mXamls.assign(xamls, xamls + size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<Stream> EmbeddedXamlProvider::LoadXaml(const char* uri)
{
    for (const auto& xaml: mXamls)
    {
        if (String::Equals(uri, xaml.name))
        {
            return *new MemoryStream(xaml.data, xaml.size);
        }
    }

    return mFallback ? mFallback->LoadXaml(uri) : nullptr;
}
