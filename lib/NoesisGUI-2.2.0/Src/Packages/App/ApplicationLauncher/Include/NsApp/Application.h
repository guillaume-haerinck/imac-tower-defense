////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_APPLICATION_H__
#define __APP_APPLICATION_H__


#include <NsCore/Noesis.h>
#include <NsCore/BaseComponent.h>
#include <NsCore/ReflectionDeclare.h>
#include <NsCore/Delegate.h>
#include <NsApp/ApplicationLauncherApi.h>
#include <NsApp/CommandLine.h>
#include <NsGui/IUITreeNode.h>
#include <NsGui/UIElementEvents.h>
#include <NsGui/Uri.h>


namespace Noesis
{

class ResourceDictionary;

}

namespace NoesisApp
{

class Display;
class Window;
class RenderContext;

NS_WARNING_PUSH
NS_MSVC_WARNING_DISABLE(4251 4275)

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Encapsulates a NoesisGUI application.
///
/// http://msdn.microsoft.com/en-us/library/system.windows.application.aspx
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_APP_APPLICATIONLAUNCHER_API Application: public Noesis::BaseComponent,
    public Noesis::IUITreeNode
{
public:
    Application();
    ~Application();

    /// Gets the current application object
    static Application* Current();

    /// Gets or sets a UI that is automatically shown when application starts
    //@{
    const Noesis::Uri& GetStartupUri() const;
    void SetStartupUri(const Noesis::Uri& startupUri);
    //@}

    /// Gets or sets a collection of application-scope resources, such as styles and brushes
    //@{
    Noesis::ResourceDictionary* GetResources() const;
    void SetResources(Noesis::ResourceDictionary* resources);
    //@}

    /// Gets the main window of the application
    Window* GetMainWindow() const;

    /// Retrieves command line arguments
    const CommandLine& GetArguments() const;

    /// Initializes the application with on the given display and command line arguments
    void Init(Display* display, const CommandLine& arguments);

    /// Ticks application
    void Tick(double time);

    /// Shuts down an application that returns the specified exit code to the operating system
    void Shutdown(int exitCode = 0);

    /// Occurs when an application becomes the foreground application
    Noesis::EventHandler& Activated();

    /// Occurs when an application stops being the foreground application
    Noesis::EventHandler& Deactivated();

    /// Occurs when the Application is initialized
    Noesis::EventHandler& StartUp();

    /// Occurs just before an application shuts down
    Noesis::EventHandler& Exit();

    /// From IUITreeNode
    //@{
    IUITreeNode* GetNodeParent() const override;
    void SetNodeParent(IUITreeNode* parent) override;
    Noesis::BaseComponent* FindNodeResource(Noesis::IResourceKey* key, bool fullSearch) const override;
    Noesis::BaseComponent* FindNodeName(const char* name) const override;
    Noesis::ObjectWithNameScope FindNodeNameAndScope(const char* name) const override;
    //@}

    NS_IMPLEMENT_INTERFACE_FIXUP

private:
    virtual const char* GetTitleOverride(Noesis::UIElement* root) const;
    virtual Noesis::Ptr<RenderContext> GetRenderContextOverride() const;
    virtual bool GetPPAAOverride() const;
    virtual uint32_t GetSamplesOverride() const;
    virtual uint32_t GetOffscreenSamplesOverride() const;
    virtual bool GetVSyncOverride() const;
    virtual bool GetsRGBOverride() const;
    void EnsureResources() const;

private:
    mutable Noesis::Ptr<Noesis::ResourceDictionary> mResources;
    IUITreeNode* mOwner;

    Noesis::Uri mStartupUri;
    Noesis::Ptr<Window> mMainWindow;
    CommandLine mArguments;

    Noesis::Ptr<RenderContext> mRenderContext;

    Noesis::EventHandler mActivated;
    Noesis::EventHandler mDeactivated;
    Noesis::EventHandler mStartUp;
    Noesis::EventHandler mExit;

    NS_DECLARE_REFLECTION(Application, BaseComponent)
};

NS_WARNING_POP

}

#endif
