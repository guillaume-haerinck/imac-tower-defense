////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_EMBEDDEDXAMLPROVIDER_H__
#define __APP_EMBEDDEDXAMLPROVIDER_H__


#include <NsCore/Noesis.h>
#include <NsCore/Vector.h>
#include <NsCore/Ptr.h>
#include <NsApp/ProvidersApi.h>
#include <NsGui/XamlProvider.h>


namespace NoesisApp
{

struct EmbeddedXaml
{
    const char* name;
    const uint8_t* data;
    uint32_t size;
};

NS_WARNING_PUSH
NS_MSVC_WARNING_DISABLE(4251 4275)

////////////////////////////////////////////////////////////////////////////////////////////////////
/// A provider for XAMLs embeded in memory
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_APP_PROVIDERS_API EmbeddedXamlProvider: public Noesis::XamlProvider
{
public:
    EmbeddedXamlProvider(const EmbeddedXaml* xamls, uint32_t size, XamlProvider* fallback = 0);

private:
    /// From XamlProvider
    //@{
    Noesis::Ptr<Noesis::Stream> LoadXaml(const char* uri) override;
    //@}

private:
    NsVector<EmbeddedXaml> mXamls;
    Noesis::Ptr<XamlProvider> mFallback;
};

NS_WARNING_POP

}

#endif
