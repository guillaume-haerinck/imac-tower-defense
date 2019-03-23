////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsApp/ApplicationLauncher.h>
#include <NsApp/Application.h>
#include <NsApp/LocalXamlProvider.h>
#include <NsApp/LocalTextureProvider.h>
#include <NsApp/LocalFontProvider.h>
#include <NsApp/EmbeddedXamlProvider.h>
#include <NsApp/EmbeddedFontProvider.h>
#include <NsApp/Window.h>
#include <NsGui/IntegrationAPI.h>
#include <NsCore/StringUtils.h>
#include <NsCore/MemoryStream.h>


using namespace Noesis;
using namespace NoesisApp;


namespace
{

// Dripicons V2 by Amit Jakhu
// https://github.com/amitjakhu/dripicons
#include "dripicons-v2.ttf.bin.h"
#include "StatsOverlay.xaml.bin.h"

}

////////////////////////////////////////////////////////////////////////////////////////////////////
ApplicationLauncher::ApplicationLauncher()
{
    String::Copy(mAppFile, sizeof(mAppFile), "");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ApplicationLauncher::~ApplicationLauncher()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationLauncher::SetApplicationFile(const char* filename)
{
    String::Copy(mAppFile, sizeof(mAppFile), filename);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationLauncher::OnStartUp()
{
    // XAML provider
    EmbeddedXaml xaml =
    {
        "_StatsOverlay.xaml", StatsOverlay_xaml, sizeof(StatsOverlay_xaml)
    };

    GUI::SetXamlProvider(MakePtr<EmbeddedXamlProvider>(&xaml, 1, GetXamlProvider()));

    // Font provider. Note that we disable system fonts because they are already being searched in
    // the font provider installed by the application. Just to avoid double searches.
    EmbeddedFont font =
    {
        "_", dripicons_v2_ttf, sizeof(dripicons_v2_ttf)
    };

    Ptr<CachedFontProvider> provider = MakePtr<EmbeddedFontProvider>(&font, 1, GetFontProvider());
    provider->SetUseSystemFonts(false);
    GUI::SetFontProvider(provider);

    // Texture provider
    GUI::SetTextureProvider(GetTextureProvider());

    // Load application file
    if (String::IsNullOrEmpty(mAppFile))
    {
        NS_FATAL("Application file not defined");
    }

    mApplication = DynamicPtrCast<Application>(GUI::LoadXaml(mAppFile));
    if (mApplication == 0)
    {
        NS_FATAL("File '%s' does not define a valid application file.", mAppFile);
    }

    mApplication->Init(GetDisplay(), GetArguments());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationLauncher::OnTick(double time)
{
    mApplication->Tick(time);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationLauncher::OnExit()
{
    mApplication.Reset();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<XamlProvider> ApplicationLauncher::GetXamlProvider() const
{
    return *new LocalXamlProvider("");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<TextureProvider> ApplicationLauncher::GetTextureProvider() const
{
    return *new LocalTextureProvider("");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<FontProvider> ApplicationLauncher::GetFontProvider() const
{
    return *new LocalFontProvider("");
}
