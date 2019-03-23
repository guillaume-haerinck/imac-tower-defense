////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsApp/Application.h>
#include <NsApp/Window.h>
#include <NsApp/Display.h>
#include <NsApp/DisplayLauncher.h>
#include <NsGui/DependencyProperty.h>
#include <NsGui/ResourceDictionary.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/IView.h>
#include <NsCore/ReflectionImplement.h>
#include <NsCore/TypeId.h>
#include <NsRender/RenderContext.h>
#include <NsRender/RenderDevice.h>
#include <EASTL/algorithm.h>


using namespace Noesis;
using namespace NoesisApp;


namespace
{

Application* gInstance;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
Application::Application(): mOwner(0)
{
    // More than one instantation is possible, for example when dropping App.xaml into XamlPlayer
    if (gInstance == 0)
    {
        gInstance = this;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Application::~Application()
{
    mExit(this, EventArgs::Empty);

    if (mMainWindow != 0)
    {
        // Make sure the View is destroyed after the Window element tree
        Ptr<IView> view(mMainWindow->GetView());

        mMainWindow->Shutdown();
        mMainWindow.Reset();
    }

    mRenderContext->Shutdown();
    mRenderContext.Reset();

    if (mResources != 0)
    {
        mResources->RemoveDictionaryChangedListeners();
        DisconnectNode(mResources, this);
        mResources.Reset();
    }

    if (gInstance == this)
    {
        gInstance = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Application* Application::Current()
{
    NS_ASSERT(gInstance != 0);
    return gInstance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const Uri& Application::GetStartupUri() const
{
    return mStartupUri;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::SetStartupUri(const Uri& startupUri)
{
    mStartupUri = startupUri;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ResourceDictionary* Application::GetResources() const
{
    EnsureResources();
    return mResources;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::SetResources(ResourceDictionary* resources)
{
    if (mResources != resources)
    {
        DisconnectNode(mResources, this);
        mResources.Reset(resources);
        ConnectNode(mResources, this);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Window* Application::GetMainWindow() const
{
    return mMainWindow;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const CommandLine& Application::GetArguments() const
{
    return mArguments;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Init(Display* display, const CommandLine& arguments)
{
    mArguments = arguments;

    // Application resources
    GUI::SetApplicationResources(GetResources());

    // Redirect integration callbacks to display
    GUI::SetSoftwareKeyboardCallback(display, [](void* user, UIElement* focused, bool open)
    {
        if (open)
        {
            ((Display*)user)->OpenSoftwareKeyboard(focused);
        }
        else
        {
            ((Display*)user)->CloseSoftwareKeyboard();
        }
    });

    GUI::SetCursorCallback(display, [](void* user, IView*, Cursor cursor)
    {
        ((Display*)user)->SetCursor(cursor);
    });

    GUI::SetOpenUrlCallback(display, [](void* user, const char* url)
    {
        ((Display*)user)->OpenUrl(url);
    });

    GUI::SetPlaySoundCallback(display, [](void* user, const char* filename, float volume)
    {
        ((Display*)user)->PlaySound(filename, volume);
    });

    // load startup URI as main window
    if (!String::IsNullOrEmpty(mStartupUri.Get()))
    {
        Ptr<BaseComponent> root = GUI::LoadXaml(mStartupUri.Get());
        mMainWindow = DynamicPtrCast<Window>(root);

        // Non window roots are allowed
        if (mMainWindow == 0)
        {
            mMainWindow = *new Window();
            mMainWindow->DependencyObject::Init();
            mMainWindow->SetContent(root);
            mMainWindow->SetTitle(GetTitleOverride(DynamicPtrCast<UIElement>(root)));
        }
    }
    else
    {
        NS_FATAL("Startup window not defined");
    }

    display->Activated() += [this](Display*)
    {
        mActivated(this, EventArgs::Empty);
    };

    display->Deactivated() += [this](Display*)
    {
        mDeactivated(this, EventArgs::Empty);
    };

    display->NativeHandleChanged() += [this](Display*, void* window)
    {
        mRenderContext->SetWindow(window);
    };

    mRenderContext = GetRenderContextOverride();
    bool ppaa = GetPPAAOverride();
    uint32_t samples = GetSamplesOverride();
    bool vSync = GetVSyncOverride();
    bool sRGB = GetsRGBOverride();

    mRenderContext->Init(display->GetNativeHandle(), samples, vSync, sRGB);
    uint32_t offscreenSamples = eastl::min_alt(GetOffscreenSamplesOverride(), samples);
    mRenderContext->GetDevice()->SetOffscreenSampleCount(offscreenSamples);

    mMainWindow->Init(display, mRenderContext, eastl::max_alt(samples, 1U), ppaa);
    mStartUp(this, EventArgs::Empty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Tick(double time)
{
    mMainWindow->Render(time);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Shutdown(int exitCode)
{
    ((DisplayLauncher*)Launcher::Current())->Quit(exitCode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
EventHandler& Application::Activated()
{
    return mActivated;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
EventHandler& Application::Deactivated()
{
    return mDeactivated;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
EventHandler& Application::StartUp()
{
    return mStartUp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
EventHandler& Application::Exit()
{
    return mExit;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
IUITreeNode* Application::GetNodeParent() const
{
    return mOwner;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::SetNodeParent(IUITreeNode* parent)
{
    mOwner = parent;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BaseComponent* Application::FindNodeResource(IResourceKey* key, bool fullElementSearch) const
{
    Ptr<BaseComponent> resource;
    if (mResources != 0 && mResources->Find(key, resource))
    {
        return resource;
    }

    if (mOwner != 0)
    {
        return mOwner->FindNodeResource(key, fullElementSearch);
    }

    return DependencyProperty::GetUnsetValue();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BaseComponent* Application::FindNodeName(const char* name) const
{
    if (mOwner != 0)
    {
        return mOwner->FindNodeName(name);
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ObjectWithNameScope Application::FindNodeNameAndScope(const char* name) const
{
    if (mOwner != 0)
    {
        return mOwner->FindNodeNameAndScope(name);
    }
    
    return ObjectWithNameScope();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const char* Application::GetTitleOverride(UIElement* root) const
{
    Window* window = DynamicCast<Window*>(root);
    return window ? window->GetTitle() : "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<RenderContext> Application::GetRenderContextOverride() const
{
    if (mArguments.HasOption("render"))
    {
        return RenderContext::Create(mArguments.FindOption("render", ""));
    }
    else
    {
        return RenderContext::Create();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool Application::GetPPAAOverride() const
{
    return GetSamplesOverride() == 1 && atoi(mArguments.FindOption("ppaa", "1")) != 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t Application::GetSamplesOverride() const
{
    return atoi(mArguments.FindOption("samples", "1"));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t Application::GetOffscreenSamplesOverride() const
{
    return GetSamplesOverride();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool Application::GetVSyncOverride() const
{
    return atoi(mArguments.FindOption("vsync", "1")) != 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool Application::GetsRGBOverride() const
{
    return mArguments.HasOption("linear");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::EnsureResources() const
{
    if (mResources == 0)
    {
        mResources = *new ResourceDictionary();
        ConnectNode(mResources, this);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
NS_BEGIN_COLD_REGION

NS_IMPLEMENT_REFLECTION(Application)
{
    NsMeta<TypeId>("Application");

    NsImpl<IUITreeNode>();

    NsProp("Resources", &Application::GetResources, &Application::SetResources);
    NsProp("MainWindow", &Application::GetMainWindow);
    NsProp("StartupUri", &Application::mStartupUri);
    NsEvent("Activated", &Application::mActivated);
    NsEvent("Deactivated", &Application::mDeactivated);
    NsEvent("StartUp", &Application::mStartUp);
    NsEvent("Exit", &Application::mExit);
}
