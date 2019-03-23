////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsApp/Window.h>
#include <NsGui/UIElementData.h>
#include <NsGui/Canvas.h>
#include <NsGui/Brushes.h>
#include <NsGui/Path.h>
#include <NsGui/TextBlock.h>
#include <NsGui/MeshGeometry.h>
#include <NsGui/IView.h>
#include <NsGui/IRenderer.h>
#include <NsGui/Brushes.h>
#include <NsGui/SolidColorBrush.h>
#include <NsGui/FrameworkPropertyMetadata.h>
#include <NsGui/ResourceKeyType.h>
#include <NsGui/ResourceKeyString.h>
#include <NsGui/ResourceDictionary.h>
#include <NsGui/Keyboard.h>
#include <NsGui/KeyboardNavigation.h>
#include <NsGui/SizeChangedInfo.h>
#include <NsGui/Popup.h>
#include <NsGui/ToggleButton.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/VisualTreeHelper.h>
#include <NsGui/Storyboard.h>
#include <NsCore/Kernel.h>
#include <NsCore/TypeId.h>
#include <NsCore/Nullable.h>
#include <NsCore/ReflectionImplement.h>
#include <NsCore/CpuProfiler.h>
#include <NsRender/RenderContext.h>
#include <NsDrawing/Color.h>


using namespace Noesis;
using namespace NoesisApp;


enum WindowFlags
{
    WindowFlags_DontUpdateDisplay = 1 << 0,
    WindowFlags_DontUpdateWindow = 1 << 1
};

#define CHECK_FLAG(f) ((mWindowFlags & f) != 0)
#define SET_FLAG(f) (mWindowFlags |= f)
#define CLEAR_FLAG(f) (mWindowFlags &= ~f);


////////////////////////////////////////////////////////////////////////////////////////////////////
Window::Window(): mDisplay(0), mWindowFlags(0), mViewFlags(0), mBackgroundCoerced(false),
    mActiveView(0), mRenderContext(0), mIsCtrlDown(false), mIsShiftDown(false),
    mStatsMode(StatsMode_Disabled), mToolbar(0), mWireframeBtn(0), mBatchesBtn(0), mOverdrawBtn(0),
    mPPAABtn(0), mStatsBtn(0), mWaitToHideToolbar(0)
{
    // Window does not use layout properties to measure or arrange
    IgnoreLayout(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Window::~Window()
{
    mClosed(this, EventArgs::Empty);

    if (mDisplay != 0)
    {
        mDisplay->LocationChanged() -= MakeDelegate(this, &Window::OnDisplayLocationChanged);
        mDisplay->SizeChanged() -= MakeDelegate(this, &Window::OnDisplaySizeChanged);
        mDisplay->FileDropped() -= MakeDelegate(this, &Window::OnDisplayFileDropped);
        mDisplay->Activated() -= MakeDelegate(this, &Window::OnDisplayActivated);
        mDisplay->Deactivated() -= MakeDelegate(this, &Window::OnDisplayDeactivated);
        mDisplay->MouseMove() -= MakeDelegate(this, &Window::OnDisplayMouseMove);
        mDisplay->MouseButtonDown() -= MakeDelegate(this, &Window::OnDisplayMouseButtonDown);
        mDisplay->MouseButtonUp() -= MakeDelegate(this, &Window::OnDisplayMouseButtonUp);
        mDisplay->MouseDoubleClick() -= MakeDelegate(this, &Window::OnDisplayMouseDoubleClick);
        mDisplay->MouseWheel() -= MakeDelegate(this, &Window::OnDisplayMouseWheel);
        mDisplay->MouseHWheel() -= MakeDelegate(this, &Window::OnDisplayMouseHWheel);
        mDisplay->Scroll() -= MakeDelegate(this, &Window::OnDisplayScroll);
        mDisplay->HScroll() -= MakeDelegate(this, &Window::OnDisplayHScroll);
        mDisplay->TouchDown() -= MakeDelegate(this, &Window::OnDisplayTouchDown);
        mDisplay->TouchMove() -= MakeDelegate(this, &Window::OnDisplayTouchMove);
        mDisplay->TouchUp() -= MakeDelegate(this, &Window::OnDisplayTouchUp);
        mDisplay->KeyDown() -= MakeDelegate(this, &Window::OnDisplayKeyDown);
        mDisplay->KeyUp() -= MakeDelegate(this, &Window::OnDisplayKeyUp);
        mDisplay->Char() -= MakeDelegate(this, &Window::OnDisplayChar);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::Init(Display* display, RenderContext* context, uint32_t samples, bool ppaa)
{
    mDisplay = display;
    mSamples = samples;
    mPPAA = ppaa;
    mRenderContext = context;

    // Set display properties
    mDisplay->SetTitle(GetTitle());
    mDisplay->SetWindowStyle(GetWindowStyle());
    mDisplay->SetWindowState(GetWindowState());
    mDisplay->SetWindowStartupLocation(GetWindowStartupLocation());
    mDisplay->SetResizeMode(GetResizeMode());
    mDisplay->SetShowInTaskbar(GetShowInTaskbar());
    mDisplay->SetTopmost(GetTopmost());
    mDisplay->SetAllowFileDrop(GetAllowDrop());

    // Location
    float left = GetLeft();
    float top = GetTop();
    if (!IsNaN(left) && !IsNaN(top))
    {
        mDisplay->SetLocation((int)left, (int)top);
    }

    // Size
    float width = GetWidth();
    float height = GetHeight();
    if (!IsNaN(width) && !IsNaN(height))
    {
        mDisplay->SetSize((uint32_t)width, (uint32_t)height);
    }

    // Create view
    mView = GUI::CreateView(this);
    mView->SetIsPPAAEnabled(ppaa);
    mView->SetTessellationMaxPixelError(TessellationMaxPixelError::HighQuality());
    mView->GetRenderer()->Init(context->GetDevice());
    mActiveView = mView;

    // Enable manipulations (to swipe toolbar)
    SetIsManipulationEnabled(true);

    // Hook to display events
    mDisplay->LocationChanged() += MakeDelegate(this, &Window::OnDisplayLocationChanged);
    mDisplay->SizeChanged() += MakeDelegate(this, &Window::OnDisplaySizeChanged);
    mDisplay->FileDropped() += MakeDelegate(this, &Window::OnDisplayFileDropped);
    mDisplay->Activated() += MakeDelegate(this, &Window::OnDisplayActivated);
    mDisplay->Deactivated() += MakeDelegate(this, &Window::OnDisplayDeactivated);
    mDisplay->MouseMove() += MakeDelegate(this, &Window::OnDisplayMouseMove);
    mDisplay->MouseButtonDown() += MakeDelegate(this, &Window::OnDisplayMouseButtonDown);
    mDisplay->MouseButtonUp() += MakeDelegate(this, &Window::OnDisplayMouseButtonUp);
    mDisplay->MouseDoubleClick() += MakeDelegate(this, &Window::OnDisplayMouseDoubleClick);
    mDisplay->MouseWheel() += MakeDelegate(this, &Window::OnDisplayMouseWheel);
    mDisplay->MouseHWheel() += MakeDelegate(this, &Window::OnDisplayMouseHWheel);
    mDisplay->Scroll() += MakeDelegate(this, &Window::OnDisplayScroll);
    mDisplay->HScroll() += MakeDelegate(this, &Window::OnDisplayHScroll);
    mDisplay->TouchDown() += MakeDelegate(this, &Window::OnDisplayTouchDown);
    mDisplay->TouchMove() += MakeDelegate(this, &Window::OnDisplayTouchMove);
    mDisplay->TouchUp() += MakeDelegate(this, &Window::OnDisplayTouchUp);
    mDisplay->KeyDown() += MakeDelegate(this, &Window::OnDisplayKeyDown);
    mDisplay->KeyUp() += MakeDelegate(this, &Window::OnDisplayKeyUp);
    mDisplay->Char() += MakeDelegate(this, &Window::OnDisplayChar);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::Render(double time)
{
    NS_PROFILE_CPU("Window/Render");

    // Make sure display invoked OnDisplaySizeChanged() callback
    NS_ASSERT(!IsNaN(GetWidth()));
    NS_ASSERT(!IsNaN(GetHeight()));

    // Sync view flags (TCP changes)
    uint32_t viewFlags = mView->GetFlags();
    bool ppaa = mView->GetIsPPAAEnabled();
    if (mViewFlags != viewFlags || mPPAA != ppaa)
    {
        mViewFlags = viewFlags;
        mPPAA = ppaa;
        SyncViewFlags();
    }

    // Update view
    UpdateStats(time);
    mView->Update(time);

    // Render view
    mRenderContext->BeginRender();
    {
        IRenderer* renderer = mView->GetRenderer();
        renderer->UpdateRenderTree();

        // Offscreen pass
        if (renderer->NeedsOffscreen())
        {
            renderer->RenderOffscreen();
        }

        // Bind main render target
        uint32_t width = mDisplay->GetClientWidth();
        uint32_t height = mDisplay->GetClientHeight();
        // TODO: When Overdraw is enabled we should clear to 0
        bool clearColor = mBackgroundCoerced || (viewFlags & (RenderFlags_Overdraw | RenderFlags_Wireframe)) > 0;
        mRenderContext->SetDefaultRenderTarget(width, height, clearColor);

        // Onscreen pass
        renderer->Render();
        RenderStats(time);
    }
    mRenderContext->EndRender();

    // Swap buffers
    {
        NS_PROFILE_CPU("App/Swap");
        mRenderContext->Swap();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::Shutdown()
{
    mView->GetRenderer()->Shutdown();
    mView.Reset();

    if (mViewStats != 0)
    {
        mViewStats->GetRenderer()->Shutdown();
        mViewStats.Reset();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::Close()
{
    if (mDisplay != 0)
    {
        mDisplay->Close();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool Window::GetAllowsTransparency() const
{
    return GetValue<bool>(AllowsTransparencyProperty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::SetAllowsTransparency(bool allowsTransparency)
{
    SetValue<bool>(AllowsTransparencyProperty, allowsTransparency);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool Window::GetFullscreen() const
{
    return GetValue<bool>(FullscreenProperty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::SetFullscreen(bool fullscreen)
{
    SetValue<bool>(FullscreenProperty, fullscreen);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool Window::GetIsActive() const
{
    return GetValue<bool>(IsActiveProperty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::SetIsActive(bool value)
{
    SetValue<bool>(IsActiveProperty, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
float Window::GetLeft() const
{
    return GetValue<float>(LeftProperty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::SetLeft(float value)
{
    SetValue<float>(LeftProperty, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ResizeMode Window::GetResizeMode() const
{
    return GetValue<ResizeMode>(ResizeModeProperty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::SetResizeMode(ResizeMode value)
{
    SetValue<ResizeMode>(ResizeModeProperty, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool Window::GetShowInTaskbar() const
{
    return GetValue<bool>(ShowInTaskbarProperty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::SetShowInTaskbar(bool value)
{
    SetValue<bool>(ShowInTaskbarProperty, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const char* Window::GetTitle() const
{
    return GetValue<NsString>(TitleProperty).c_str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::SetTitle(const char* value)
{
    SetValue<NsString>(TitleProperty, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
float Window::GetTop() const
{
    return GetValue<float>(TopProperty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::SetTop(float value)
{
    SetValue<float>(TopProperty, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool Window::GetTopmost() const
{
    return GetValue<bool>(TopmostProperty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::SetTopmost(bool value)
{
    SetValue<bool>(TopmostProperty, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
WindowState Window::GetWindowState() const
{
    return GetValue<WindowState>(WindowStateProperty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::SetWindowState(WindowState value)
{
    SetValue<WindowState>(WindowStateProperty, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
WindowStyle Window::GetWindowStyle() const
{
    return GetValue<WindowStyle>(WindowStyleProperty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::SetWindowStyle(WindowStyle value)
{
    SetValue<WindowStyle>(WindowStyleProperty, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
WindowStartupLocation Window::GetWindowStartupLocation() const
{
    return GetValue<WindowStartupLocation>(WindowStartupLocationProperty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::SetWindowStartupLocation(WindowStartupLocation location)
{
    SetValue<WindowStartupLocation>(WindowStartupLocationProperty, location);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
EventHandler& Window::Activated()
{
    return mActivated;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
EventHandler& Window::Deactivated()
{
    return mDeactivated;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
EventHandler& Window::Closed()
{
    return mClosed;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
EventHandler& Window::LocationChanged()
{
    return mLocationChanged;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
EventHandler& Window::StateChanged()
{
    return mStateChanged;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnFullscreenChanged(bool)
{
    CoerceValue<WindowStyle>(WindowStyleProperty);
    CoerceValue<ResizeMode>(ResizeModeProperty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnTitleChanged(const char* title)
{
    if (!CHECK_FLAG(WindowFlags_DontUpdateDisplay))
    {
        SET_FLAG(WindowFlags_DontUpdateWindow);
        if (mDisplay != 0)
        {
            mDisplay->SetTitle(title);
        }
        CLEAR_FLAG(WindowFlags_DontUpdateWindow);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnLeftChanged(float left)
{
    if (!CHECK_FLAG(WindowFlags_DontUpdateDisplay))
    {
        SET_FLAG(WindowFlags_DontUpdateWindow);
        if (mDisplay != 0)
        {
            mDisplay->SetLocation((int)left, (int)GetTop());
        }
        CLEAR_FLAG(WindowFlags_DontUpdateWindow);
    }

    mLocationChanged(this, EventArgs::Empty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnTopChanged(float top)
{
    if (!CHECK_FLAG(WindowFlags_DontUpdateDisplay))
    {
        SET_FLAG(WindowFlags_DontUpdateWindow);
        if (mDisplay != 0)
        {
            mDisplay->SetLocation((int)GetLeft(), (int)top);
        }
        CLEAR_FLAG(WindowFlags_DontUpdateWindow);
    }

    mLocationChanged(this, EventArgs::Empty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnWindowStyleChanged(WindowStyle windowStyle)
{
    if (!CHECK_FLAG(WindowFlags_DontUpdateDisplay))
    {
        SET_FLAG(WindowFlags_DontUpdateWindow);
        if (mDisplay != 0)
        {
            mDisplay->SetWindowStyle(windowStyle);
        }
        CLEAR_FLAG(WindowFlags_DontUpdateWindow);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnStateChanged(WindowState windowState)
{
    if (!CHECK_FLAG(WindowFlags_DontUpdateDisplay))
    {
        SET_FLAG(WindowFlags_DontUpdateWindow);
        if (mDisplay != 0)
        {
            mDisplay->SetWindowState(windowState);
        }
        CLEAR_FLAG(WindowFlags_DontUpdateWindow);
    }

    mStateChanged(this, EventArgs::Empty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnResizeModeChanged(ResizeMode resizeMode)
{
    if (!CHECK_FLAG(WindowFlags_DontUpdateDisplay))
    {
        SET_FLAG(WindowFlags_DontUpdateWindow);
        if (mDisplay != 0)
        {
            mDisplay->SetResizeMode(resizeMode);
        }
        CLEAR_FLAG(WindowFlags_DontUpdateWindow);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnShowInTaskbarChanged(bool showInTaskbar)
{
    if (!CHECK_FLAG(WindowFlags_DontUpdateDisplay))
    {
        SET_FLAG(WindowFlags_DontUpdateWindow);
        if (mDisplay != 0)
        {
            mDisplay->SetShowInTaskbar(showInTaskbar);
        }
        CLEAR_FLAG(WindowFlags_DontUpdateWindow);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnTopmostChanged(bool topmost)
{
    if (!CHECK_FLAG(WindowFlags_DontUpdateDisplay))
    {
        SET_FLAG(WindowFlags_DontUpdateWindow);
        if (mDisplay != 0)
        {
            mDisplay->SetTopmost(topmost);
        }
        CLEAR_FLAG(WindowFlags_DontUpdateWindow);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnFileDropped(const char*)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::PreviewToolbar()
{
    // Shows Toolbar and enables it
    ShowToolbar(true);

    // Waits 2 seconds to allow user to interact with the Toolbar
    mWaitToHideToolbar->Begin(mToolbar);

    // If no interaction occurred, we hide the Toolbar
    mWaitToHideToolbar->Completed() += MakeDelegate(this, &Window::HideToolbar);

    // Detect if user interacted with the Toolbar
    mToolbar->MouseEnter() += MakeDelegate(this, &Window::OnToolbarMouseInteraction);
    mToolbar->TouchEnter() += MakeDelegate(this, &Window::OnToolbarTouchInteraction);
    mToolbar->PreviewKeyDown() += MakeDelegate(this, &Window::OnToolbarKeyInteraction);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::HideToolbar(BaseComponent*, const TimelineEventArgs&)
{
    mWaitToHideToolbar->Completed() -= MakeDelegate(this, &Window::HideToolbar);
    ShowToolbar(false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnToolbarMouseInteraction(BaseComponent*, const MouseEventArgs&)
{
    mToolbar->MouseEnter() -= MakeDelegate(this, &Window::OnToolbarMouseInteraction);
    mWaitToHideToolbar->Completed() -= MakeDelegate(this, &Window::HideToolbar);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnToolbarTouchInteraction(BaseComponent*, const TouchEventArgs&)
{
    mToolbar->TouchEnter() -= MakeDelegate(this, &Window::OnToolbarTouchInteraction);
    mWaitToHideToolbar->Completed() -= MakeDelegate(this, &Window::HideToolbar);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnToolbarKeyInteraction(BaseComponent*, const KeyEventArgs&)
{
    mToolbar->PreviewKeyDown() -= MakeDelegate(this, &Window::OnToolbarKeyInteraction);
    mWaitToHideToolbar->Completed() -= MakeDelegate(this, &Window::HideToolbar);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool Window::OnPropertyChanged(const DependencyPropertyChangedEventArgs& args)
{
    bool handled = ParentClass::OnPropertyChanged(args);

    if (!handled)
    {
        if (args.prop == FullscreenProperty)
        {
            OnFullscreenChanged(*static_cast<const bool*>(args.newValue));
            return true;
        }
        if (args.prop == TitleProperty)
        {
            OnTitleChanged(static_cast<const NsString*>(args.newValue)->c_str());
            return true;
        }
        if (args.prop == LeftProperty)
        {
            OnLeftChanged(*static_cast<const float*>(args.newValue));
            return true;
        }
        if (args.prop == TopProperty)
        {
            OnTopChanged(*static_cast<const float*>(args.newValue));
            return true;
        }
        if (args.prop == WindowStyleProperty)
        {
            OnWindowStyleChanged(*static_cast<const WindowStyle*>(args.newValue));
            return true;
        }
        if (args.prop == WindowStateProperty)
        {
            OnStateChanged(*static_cast<const WindowState*>(args.newValue));
            return true;
        }
        if (args.prop == ResizeModeProperty)
        {
            OnResizeModeChanged(*static_cast<const ResizeMode*>(args.newValue));
            return true;
        }
        if (args.prop == ShowInTaskbarProperty)
        {
            OnShowInTaskbarChanged(*static_cast<const bool*>(args.newValue));
            return true;
        }
        if (args.prop == TopmostProperty)
        {
            OnTopmostChanged(*static_cast<const bool*>(args.newValue));
            return true;
        }
    }

    return handled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnWidthChanged(float width)
{
    ParentClass::OnWidthChanged(width);

    if (!CHECK_FLAG(WindowFlags_DontUpdateDisplay))
    {
        SET_FLAG(WindowFlags_DontUpdateWindow);
        if (mDisplay != 0)
        {
            mDisplay->SetSize((uint32_t)width, (uint32_t)GetHeight());
        }
        CLEAR_FLAG(WindowFlags_DontUpdateWindow);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnHeightChanged(float height)
{
    ParentClass::OnHeightChanged(height);

    if (!CHECK_FLAG(WindowFlags_DontUpdateDisplay))
    {
        SET_FLAG(WindowFlags_DontUpdateWindow);
        if (mDisplay != 0)
        {
            mDisplay->SetSize((uint32_t)GetWidth(), (uint32_t)height);
        }
        CLEAR_FLAG(WindowFlags_DontUpdateWindow);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Size Window::MeasureOverride(const Size& availableSize)
{
    Size desiredSize = ParentClass::MeasureOverride(availableSize);

    if (mDisplay != 0)
    {
        return Sizei((int)GetWidth(), (int)GetHeight());
    }

    return desiredSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Size Window::ArrangeOverride(const Size& finalSize)
{
    ParentClass::ArrangeOverride(finalSize);

    if (mDisplay != 0)
    {
        return Sizei((int)GetWidth(), (int)GetHeight());
    }

    return finalSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnManipulationStarted(const ManipulationStartedEventArgs& e)
{
    if (e.source == this)
    {
        if (mDisplay == 0 || e.manipulationOrigin.x > mDisplay->GetClientWidth() * 0.1f)
        {
            // Finger was touching outside our swipe area (10% of left screen)
            e.cancel = true;
        }

        e.handled = true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnManipulationInertiaStarting(const ManipulationInertiaStartingEventArgs& e)
{
    if (e.source == this)
    {
        if (mDisplay != 0 && (mToolbar == 0 || mToolbar->GetIsEnabled() == false))
        {
            // Detect if user moved his finger upwards
            if (e.initialVelocities.linearVelocity.x > 0.5f)
            {
                ShowToolbar(true);
            }
        }

        e.handled = true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::HitTestToolbar(int x, int y)
{
    if (mToolbar != 0 && mToolbar->GetIsEnabled() == true)
    {
        Pointi p(x ,y);
        if (VisualTreeHelper::HitTest(mToolbar, mToolbar->PointFromScreen(p)).visualHit == 0)
        {
            ShowToolbar(false);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::ShowToolbar(bool visible)
{
    LoadOverlayXAML();
    NS_ASSERT(mToolbar != 0);

    if (visible)
    {
        mToolbar->SetIsEnabled(true);
#if defined(NS_PLATFORM_XBOX_ONE) || defined(NS_PLATFORM_PS4)
        mWireframeBtn->Focus();
#endif
        mActiveView = mViewStats;
        SyncViewFlags();
    }
    else
    {
        mToolbar->SetIsEnabled(false);
        mActiveView = mView;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::EnablePPAA(BaseComponent*, const RoutedEventArgs&)
{
    mPPAA = true;
    SyncViewFlags();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::DisablePPAA(BaseComponent*, const RoutedEventArgs&)
{
    mPPAA = false;
    SyncViewFlags();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::EnableWireframe(BaseComponent*, const RoutedEventArgs&)
{
    mViewFlags = RenderFlags_Wireframe;
    SyncViewFlags();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::DisableWireframe(BaseComponent*, const RoutedEventArgs&)
{
    mViewFlags = mViewFlags & ~RenderFlags_Wireframe;
    mView->SetFlags(mViewFlags);

    if (mWireframeBtn != 0)
    {
        mWireframeBtn->SetIsChecked(false);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Window::EnableBatches(BaseComponent*, const RoutedEventArgs&)
{
    mViewFlags = RenderFlags_ColorBatches;
    SyncViewFlags();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Window::DisableBatches(BaseComponent*, const RoutedEventArgs&)
{
    mViewFlags = mViewFlags & ~RenderFlags_ColorBatches;
    mView->SetFlags(mViewFlags);

    if (mBatchesBtn != 0)
    {
        mBatchesBtn->SetIsChecked(false);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Window::EnableOverdraw(BaseComponent*, const RoutedEventArgs&)
{
    mViewFlags = RenderFlags_Overdraw;
    SyncViewFlags();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Window::DisableOverdraw(BaseComponent*, const RoutedEventArgs&)
{
    mViewFlags = mViewFlags & ~RenderFlags_Overdraw;
    mView->SetFlags(mViewFlags);

    if (mOverdrawBtn != 0)
    {
        mOverdrawBtn->SetIsChecked(false);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Window::EnableTitleStats(BaseComponent*, const RoutedEventArgs&)
{
    mStatsMode = StatsMode_TitleBar;
    String::Copy(mCachedTitle, sizeof(mCachedTitle), GetTitle());

    if (mStats != 0)
    {
        mStats->SetVisibility(Visibility_Hidden);
    }

    if (mStatsBtn != 0)
    {
        mStatsBtn->SetIsChecked(nullptr);
    }

    mLastRenderTime = 0.0;
    mNumFrames = 0;
    mUpdateTime = 0.0f; 
    mRenderTime = 0.0f;
    mFrameTime = 0.0f;
    mGPUTime = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Window::EnableWidgetStats(BaseComponent*, const RoutedEventArgs&)
{
    mStatsMode = StatsMode_Widget;
    String::Copy(mCachedTitle, sizeof(mCachedTitle), GetTitle());

    LoadOverlayXAML();
    mStats->SetVisibility(Visibility_Visible);
    mStatsBtn->SetIsChecked(true);

    mHistPos = 0;
    memset(mHistogramCPU, 0, sizeof(mHistogramCPU));
    memset(mHistogramGPU, 0, sizeof(mHistogramGPU));

    mLastRenderTime = 0.0;
    mNumFrames = 0;
    mUpdateTime = 0.0f; 
    mRenderTime = 0.0f;
    mFrameTime = 0.0f;
    mGPUTime = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Window::DisableStats(BaseComponent*, const RoutedEventArgs&)
{
    mStatsMode = StatsMode_Disabled;

    if (mDisplay != 0)
    {
        mDisplay->SetTitle(mCachedTitle);
    }

    if (mStats != 0)
    {
        mStats->SetVisibility(Visibility_Hidden);
    }

    if (mStatsBtn != 0)
    {
        mStatsBtn->SetIsChecked(false);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Window::SyncViewFlags()
{
    mView->SetFlags(mViewFlags);
    mView->SetIsPPAAEnabled(mPPAA);

    if (mWireframeBtn != 0)
    {
        mWireframeBtn->SetIsChecked((mViewFlags & RenderFlags_Wireframe) > 0);
    }

    if (mBatchesBtn != 0)
    {
        mBatchesBtn->SetIsChecked((mViewFlags & RenderFlags_ColorBatches) > 0);
    }

    if (mOverdrawBtn != 0)
    {
        mOverdrawBtn->SetIsChecked((mViewFlags & RenderFlags_Overdraw) > 0);
    }

    if (mPPAABtn != 0)
    {
        mPPAABtn->SetIsChecked(mPPAA);
    }

    if (mStatsBtn != 0)
    {
        switch (mStatsMode)
        {
            case StatsMode_Disabled:
            {
                mStatsBtn->SetIsChecked(false);
                break;
            }
            case StatsMode_TitleBar:
            {
                mStatsBtn->SetIsChecked(nullptr);
                break;
            }
            case StatsMode_Widget:
            {
                mStatsBtn->SetIsChecked(true);
                break;
            }
            default:
                NS_ASSERT_UNREACHABLE;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Window::LoadOverlayXAML()
{
    if (mViewStats == 0)
    {
        Ptr<FrameworkElement> xaml = GUI::LoadXaml<FrameworkElement>("_StatsOverlay.xaml");

        mViewStats = GUI::CreateView(xaml);
        mViewStats->SetSize(mDisplay->GetClientWidth(), mDisplay->GetClientHeight());
        mViewStats->GetRenderer()->Init(mRenderContext->GetDevice());

        mStats = xaml->FindName<FrameworkElement>("stats");
        NS_ASSERT(mStats != 0);
        mStats->SetVisibility(Visibility_Hidden);

        mToolbar = xaml->FindName<FrameworkElement>("toolbar");
        NS_ASSERT(mToolbar != 0);
        mToolbar->SetIsEnabled(false);

        mWireframeBtn = xaml->FindName<ToggleButton>("wireframeBtn");
        NS_ASSERT(mWireframeBtn != 0);
        mWireframeBtn->Checked() += MakeDelegate(this, &Window::EnableWireframe);
        mWireframeBtn->Unchecked() += MakeDelegate(this, &Window::DisableWireframe);

        mBatchesBtn = xaml->FindName<ToggleButton>("batchesBtn");
        NS_ASSERT(mBatchesBtn != 0);
        mBatchesBtn->Checked() += MakeDelegate(this, &Window::EnableBatches);
        mBatchesBtn->Unchecked() += MakeDelegate(this, &Window::DisableBatches);

        mOverdrawBtn = xaml->FindName<ToggleButton>("overdrawBtn");
        NS_ASSERT(mOverdrawBtn != 0);
        mOverdrawBtn->Checked() += MakeDelegate(this, &Window::EnableOverdraw);
        mOverdrawBtn->Unchecked() += MakeDelegate(this, &Window::DisableOverdraw);

        mPPAABtn = xaml->FindName<ToggleButton>("ppaaBtn");
        NS_ASSERT(mPPAABtn != 0);
        mPPAABtn->Checked() += MakeDelegate(this, &Window::EnablePPAA);
        mPPAABtn->Unchecked() += MakeDelegate(this, &Window::DisablePPAA);

        mStatsBtn = xaml->FindName<ToggleButton>("statsBtn");
        NS_ASSERT(mStatsBtn != 0);
        mStatsBtn->Checked() += MakeDelegate(this, &Window::EnableWidgetStats);
        mStatsBtn->Indeterminate() += MakeDelegate(this, &Window::EnableTitleStats);
        mStatsBtn->Unchecked() += MakeDelegate(this, &Window::DisableStats);

#if defined(NS_PLATFORM_WINDOWS_DESKTOP) || defined(NS_PLATFORM_OSX) || defined(NS_PLATFORM_LINUX)
        mWireframeBtn->SetTag("WIREFRAME\n\nCTRL + W");
        mBatchesBtn->SetTag("BATCHES\n\nCTRL + B");
        mOverdrawBtn->SetTag("OVERDRAW\n\nCTRL + O");
        mPPAABtn->SetTag("PPAA\n\nCTRL + P");
        mStatsBtn->SetTag("STATS\n\nCTRL + F");
        mStatsBtn->SetIsThreeState(true);
#endif

        mWaitToHideToolbar = xaml->GetResources()->FindName<Storyboard>("WaitToHideToolbar");
        NS_ASSERT(mWaitToHideToolbar != 0);

        mTextTitle = xaml->FindName<TextBlock>("title");
        mTextFps = xaml->FindName<TextBlock>("fps");
        mTextMs = xaml->FindName<TextBlock>("ms");
        mTextUpdate = xaml->FindName<TextBlock>("update");
        mTextRender = xaml->FindName<TextBlock>("render");
        mTextGPU = xaml->FindName<TextBlock>("gpu");
        mTextPrimitives = xaml->FindName<TextBlock>("primitives");
        mTextTriangles = xaml->FindName<TextBlock>("triangles");
        mTextNodes = xaml->FindName<TextBlock>("nodes");
        mTextUploads = xaml->FindName<TextBlock>("uploads");
        mTextMemory = xaml->FindName<TextBlock>("memory");

        NS_ASSERT(mTextTitle != 0);
        NS_ASSERT(mTextFps != 0);
        NS_ASSERT(mTextMs != 0);
        NS_ASSERT(mTextUpdate != 0);
        NS_ASSERT(mTextRender != 0);
        NS_ASSERT(mTextGPU != 0);
        NS_ASSERT(mTextPrimitives != 0);
        NS_ASSERT(mTextTriangles != 0);
        NS_ASSERT(mTextNodes != 0);
        NS_ASSERT(mTextUploads != 0);
        NS_ASSERT(mTextMemory != 0);

        Path* pathHistCPU = xaml->FindName<Path>("histCPU");
        NS_ASSERT(pathHistCPU != 0);
        Ptr<MeshGeometry> geoHistCPU = *new MeshGeometry();
        pathHistCPU->SetData(geoHistCPU);
        mGeoHistCPU = geoHistCPU;

        Path* pathHistGPU = xaml->FindName<Path>("histGPU");
        NS_ASSERT(pathHistGPU != 0);
        Ptr<MeshGeometry> geoHistGPU = *new MeshGeometry();
        pathHistGPU->SetData(geoHistGPU);
        mGeoHistGPU = geoHistGPU;

        FrameworkElement* histBox = xaml->FindName<FrameworkElement>("histBox");
        NS_ASSERT(histBox != 0);
        mHistSize.width = histBox->GetWidth();
        NS_ASSERT(!IsNaN(mHistSize.width));
        mHistSize.height = histBox->GetHeight();
        NS_ASSERT(!IsNaN(mHistSize.height));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Window::UpdateHistogram(uint16_t* data, float maxF, MeshGeometry* geometry) const
{
    geometry->SetBounds(Rect(mHistSize));

    float stepX = mHistSize.width / (NS_COUNTOF(mHistogramCPU) - 1);
    float baseH = mHistSize.height;
    float minH = baseH - 1.0f;

    geometry->SetNumVertices(2 * NS_COUNTOF(mHistogramCPU));
    Point* vertices = geometry->GetVertices();

    for (uint32_t i = 0; i < NS_COUNTOF(mHistogramCPU); i++)
    {
        float x = i * stepX;

        uint16_t f = data[(mHistPos + i) % NS_COUNTOF(mHistogramCPU)];
        float h = minH - (f * minH / maxF);

        vertices[2 * i] = Point(x, baseH);
        vertices[2 * i + 1] = Point(x, h);
    }

    geometry->SetNumIndices(6 * (NS_COUNTOF(mHistogramCPU) - 1));
    uint16_t* indices = geometry->GetIndices();

    for (uint16_t i = 0; i < NS_COUNTOF(mHistogramCPU) - 1; i++)
    {
        indices[6 * i] = 2 * i;
        indices[6 * i + 1] = 2 * i + 1;
        indices[6 * i + 2] = 2 * i + 2;

        indices[6 * i + 3] = 2 * i + 2;
        indices[6 * i + 4] = 2 * i + 1;
        indices[6 * i + 5] = 2 * i + 3;
    }

    geometry->Update();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::UpdateStats(double time)
{
    if (mStatsMode != StatsMode_Disabled)
    {
        ViewStats stats = mView->GetStats();
        mUpdateTime += stats.updateTime;
        mRenderTime += stats.renderTime;
        mFrameTime += stats.frameTime;
        mGPUTime += mRenderContext->GetGPUTimeMs();
        mNumFrames++;

        float dt = (float)(time - mLastRenderTime);

        if (dt > 0.5f)
        {
            float update = mUpdateTime / mNumFrames;
            float render = mRenderTime / mNumFrames;
            float frame = mFrameTime / mNumFrames;
            float gpu = mGPUTime / mNumFrames;
            float fps = IsZero(frame) ? 0.0f : 1000.0f / frame;

            mLastRenderTime = time;
            mNumFrames = 0;
            mUpdateTime = 0.0f;
            mRenderTime = 0.0f;
            mFrameTime = 0.0f;
            mGPUTime = 0.0f;

            if (mStatsMode == StatsMode_TitleBar)
            {
                char title[512];
                snprintf(title, sizeof(title), "%s [%.0f fps %.2f ms (%.2f %.2f %.2f)]",
                    mCachedTitle, fps, frame, update, render, gpu);
                mDisplay->SetTitle(title);
            }
            else
            {
                char buffer[64];
                snprintf(buffer, 64, "%s - %ux%u - %dx", mRenderContext->Description(),
                    mDisplay->GetClientWidth(), mDisplay->GetClientHeight(), mSamples);
                String::Append(buffer, 64, mPPAA ? " - PPAA" : "");
                mTextTitle->SetText(buffer);

                snprintf(buffer, 64, "%.0f fps", fps);
                mTextFps->SetText(buffer);

                snprintf(buffer, 64, "%.2f ms", frame);
                mTextMs->SetText(buffer);

                snprintf(buffer, 64, "%5.2f ms", update);
                mTextUpdate->SetText(buffer);

                snprintf(buffer, 64, "%5.2f ms", render);
                mTextRender->SetText(buffer);

                snprintf(buffer, 64, "%5.2f ms", gpu);
                mTextGPU->SetText(buffer);

                snprintf(buffer, 64, "Tris: %6d  Draws: %6d  Batches: %5d", stats.triangles,
                    stats.draws, stats.batches);
                mTextTriangles->SetText(buffer);

                snprintf(buffer, 64, "Tess: %6d  Flushes: %4d  Size: %5d KB", stats.tessellations,
                    stats.flushes, stats.geometrySize / 1024);
                mTextPrimitives->SetText(buffer);

                snprintf(buffer, 64, "Masks: %5d  Tiles: %6d  SetRTs: %6d", stats.masks,
                    stats.opacities, stats.renderTargetSwitches);
                mTextNodes->SetText(buffer);

                snprintf(buffer, 64, "Ramps^ %5d  Glyphs^ %5d  Discards: %4d", stats.uploadedRamps,
                    stats.rasterizedGlyphs, stats.discardedGlyphTiles);
                mTextUploads->SetText(buffer);

                float mem = GetAllocatedMemory() / (1024.0f * 1024.0f);
                uint32_t allocsAccum = GetAllocationsCount();

                snprintf(buffer, 64, "Memory %.2f MB (%d allocs)", mem, allocsAccum);
                mTextMemory->SetText(buffer);

                static_assert(NS_COUNTOF(mHistogramCPU) == NS_COUNTOF(mHistogramGPU), "array size");
                mHistogramCPU[mHistPos] = (uint16_t)(frame * 100.0f);
                mHistogramGPU[mHistPos] = (uint16_t)(gpu * 100.0f);
                mHistPos = (mHistPos + 1) % NS_COUNTOF(mHistogramCPU);

                float maxF = 0;
                for (uint32_t i = 0; i < NS_COUNTOF(mHistogramCPU); i++)
                {
                    float cpuF = mHistogramCPU[i];
                    float gpuF = mHistogramGPU[i];
                    maxF = Max(Max(maxF, cpuF), gpuF);
                }

                maxF = maxF > 0.0f ? (maxF * 110.0f) / 100.0f : 1.0f;

                UpdateHistogram(mHistogramCPU, maxF, mGeoHistCPU);
                UpdateHistogram(mHistogramGPU, maxF, mGeoHistGPU);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::RenderStats(double time)
{
    if (mViewStats != 0)
    {
        NS_PROFILE_CPU("Window/RenderStats");

        mViewStats->Update(time);

        IRenderer* renderer = mViewStats->GetRenderer();
        renderer->UpdateRenderTree();
        NS_ASSERT(!renderer->NeedsOffscreen());
        renderer->Render();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayLocationChanged(Display*, int x, int y)
{
    if (!CHECK_FLAG(WindowFlags_DontUpdateWindow))
    {
        SET_FLAG(WindowFlags_DontUpdateDisplay);
        SetLeft((float)x);
        SetTop((float)y);
        CLEAR_FLAG(WindowFlags_DontUpdateDisplay);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplaySizeChanged(Display*, uint32_t width, uint32_t height)
{
    if (!CHECK_FLAG(WindowFlags_DontUpdateWindow))
    {
        SET_FLAG(WindowFlags_DontUpdateDisplay);
        SetWidth((float)width);
        SetHeight((float)height);
        CLEAR_FLAG(WindowFlags_DontUpdateDisplay);
    }

    if (mView != 0)
    {
        NS_ASSERT(mRenderContext != 0);
        mRenderContext->Resize();

        NS_ASSERT(mDisplay != 0);
        uint32_t cw = mDisplay->GetClientWidth();
        uint32_t ch = mDisplay->GetClientHeight();

        mView->SetSize(cw, ch);

        if (mViewStats != 0)
        {
            mViewStats->SetSize(cw, ch);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayFileDropped(Display*, const char* filename)
{
    OnFileDropped(filename);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayActivated(Display*)
{
    mView->Activate();
    SetReadOnlyProperty<bool>(IsActiveProperty, true);
    mActivated(this, EventArgs::Empty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayDeactivated(Display*)
{
    mView->Deactivate();
    mIsCtrlDown = false;
    mIsShiftDown = false;
    SetReadOnlyProperty<bool>(IsActiveProperty, false);
    mDeactivated(this, EventArgs::Empty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayMouseMove(Display*, int x, int y)
{
    mActiveView->MouseMove(x, y);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayMouseButtonDown(Display*, int x, int y, MouseButton button)
{
    HitTestToolbar(x, y);
    mActiveView->MouseButtonDown(x, y, button);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayMouseButtonUp(Display*, int x, int y, MouseButton button)
{
    mActiveView->MouseButtonUp(x, y, button);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayMouseDoubleClick(Display*, int x, int y, MouseButton button)
{
    mActiveView->MouseDoubleClick(x, y, button);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayMouseWheel(Display*, int x, int y, int delta)
{
    mActiveView->MouseWheel(x, y, delta);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayMouseHWheel(Display*, int x, int y, int delta)
{
    mActiveView->MouseHWheel(x, y, delta);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayScroll(Display*, float value)
{
    mActiveView->Scroll(value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayHScroll(Display*, float value)
{
    mActiveView->HScroll(value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayTouchDown(Display*, int x, int y, uint64_t id)
{
    HitTestToolbar(x ,y);
    mActiveView->TouchDown(x, y, id);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayTouchMove(Display*, int x, int y, uint64_t id)
{
    mActiveView->TouchMove(x, y, id);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayTouchUp(Display*, int x, int y, uint64_t id)
{
    mActiveView->TouchUp(x, y, id);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayKeyDown(Display*, Key key)
{
    mActiveView->KeyDown(key);

    mIsCtrlDown = key == Key_LeftCtrl ? true : mIsCtrlDown;
    mIsShiftDown = key == Key_LeftShift ? true : mIsShiftDown;

    if ((mIsCtrlDown && key == Key_T) || key == Key_GamepadMenu)
    {
        ShowToolbar(mToolbar ? !mToolbar->GetIsEnabled() : true);
    }
    else if (mIsCtrlDown && key == Key_F)
    {
        if (mStatsMode == StatsMode_Disabled)
        {
            EnableWidgetStats(0, RoutedEventArgs(0, 0));
        }
        else if (mStatsMode == StatsMode_Widget)
        {
            EnableTitleStats(0, RoutedEventArgs(0, 0));
        }
        else
        {
            DisableStats(0, RoutedEventArgs(0, 0));
        }
    }
    else if (mIsCtrlDown && key == Key_P)
    {
        if (mPPAA)
        {
            DisablePPAA(0, RoutedEventArgs(0, 0));
        }
        else
        {
            EnablePPAA(0, RoutedEventArgs(0, 0));
        }
    }
    else if (mIsCtrlDown && key == Key_W)
    {
        if ((mViewFlags & RenderFlags_Wireframe) > 0)
        {
            DisableWireframe(0, RoutedEventArgs(0, 0));
        }
        else
        {
            EnableWireframe(0, RoutedEventArgs(0, 0));
        }
    }
    else if (mIsCtrlDown && key == Key_B)
    {
        if ((mViewFlags & RenderFlags_ColorBatches) > 0)
        {
            DisableBatches(0, RoutedEventArgs(0, 0));
        }
        else
        {
            EnableBatches(0, RoutedEventArgs(0, 0));
        }
    }
    else if (mIsCtrlDown && key == Key_O)
    {
        if ((mViewFlags & RenderFlags_Overdraw) > 0)
        {
            DisableOverdraw(0, RoutedEventArgs(0, 0));
        }
        else
        {
            EnableOverdraw(0, RoutedEventArgs(0, 0));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayKeyUp(Display*, Key key)
{
    mIsCtrlDown = key == Key_LeftCtrl ? false : mIsCtrlDown;
    mIsShiftDown = key == Key_LeftShift ? false : mIsShiftDown;

    mActiveView->KeyUp(key);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Window::OnDisplayChar(Display*, uint32_t c)
{
    mActiveView->Char(c);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool Window::CoerceWindowStyle(const DependencyObject* object, const void* in, void* out)
{
    const Window* window = static_cast<const Window*>(object);
    WindowStyle newValue = *static_cast<const WindowStyle*>(in);
    WindowStyle& coerced = *static_cast<WindowStyle*>(out);

    coerced = window->GetFullscreen() ? WindowStyle_None : newValue;

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool Window::CoerceResizeMode(const DependencyObject* object, const void* in, void* out)
{
    const Window* window = static_cast<const Window*>(object);
    ResizeMode newValue = *static_cast<const ResizeMode*>(in);
    ResizeMode& coerced = *static_cast<ResizeMode*>(out);

    coerced = window->GetFullscreen() ? ResizeMode_NoResize : newValue;

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool Window::CoerceBackground(const DependencyObject* object, const void* in, void* out)
{
    const Window* window = static_cast<const Window*>(object);
    Brush* newValue = static_cast<const Ptr<Brush>*>(in)->GetPtr();
    Ptr<Brush>& coerced = *static_cast<Ptr<Brush>*>(out);
    SolidColorBrush* clearColor = DynamicCast<SolidColorBrush*>(newValue);

    // Doing a color clear is usually faster than drawing a rectangle, so in case the window has
    // a solid background we disable it and transfer to the render context
    if (clearColor != 0)
    {
        RenderContext* renderContext = window->mRenderContext;
        if (renderContext != 0)
        {
            Color c = clearColor->GetColor();
            renderContext->SetClearColor(c.GetRedF(), c.GetGreenF(), c.GetBlueF(), c.GetAlphaF());
        }

        // We set a transparent brush so HitTesting works on the Window
        coerced.Reset(Brushes::Transparent());

        window->mBackgroundCoerced = true;
        return true;
    }

    window->mBackgroundCoerced = false;
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
NS_BEGIN_COLD_REGION

NS_IMPLEMENT_REFLECTION(Window)
{
    NsMeta<TypeId>("Window");

    const TypeClass* type = TypeOf<SelfClass>();
    Ptr<ResourceKeyType> defaultStyleKey = ResourceKeyType::Create(type);

    // register properties and events
    UIElementData* data = NsMeta<UIElementData>(TypeOf<SelfClass>());

    data->AddOwner<bool>(AllowsTransparencyProperty, "AllowsTransparency",
        Popup::AllowsTransparencyProperty);
    data->RegisterProperty<bool>(FullscreenProperty, "Fullscreen",
        FrameworkPropertyMetadata::Create(false, FrameworkPropertyMetadataOptions_AffectsMeasure |
        FrameworkPropertyMetadataOptions_AffectsRender));
    data->RegisterProperty<bool>(IsActiveProperty, "IsActive",
        FrameworkPropertyMetadata::Create(false));
    data->AddOwner<float>(LeftProperty, "Left", Canvas::LeftProperty);
    data->RegisterProperty<ResizeMode>(ResizeModeProperty, "ResizeMode",
        FrameworkPropertyMetadata::Create(ResizeMode_CanResize,
            CoerceValueCallback(CoerceResizeMode)));
    data->RegisterProperty<bool>(ShowInTaskbarProperty, "ShowInTaskbar",
        FrameworkPropertyMetadata::Create(true));
    data->RegisterProperty<NsString>(TitleProperty, "Title",
        FrameworkPropertyMetadata::Create(NsString()));
    data->AddOwner<float>(TopProperty, "Top", Canvas::TopProperty);
    data->RegisterProperty<bool>(TopmostProperty, "Topmost",
        FrameworkPropertyMetadata::Create(false));
    data->RegisterProperty<WindowState>(WindowStateProperty, "WindowState",
        FrameworkPropertyMetadata::Create(WindowState_Normal));
    data->RegisterProperty<WindowStyle>(WindowStyleProperty, "WindowStyle",
        FrameworkPropertyMetadata::Create(WindowStyle_SingleBorderWindow,
            CoerceValueCallback(CoerceWindowStyle)));
    data->RegisterProperty<WindowStartupLocation>(WindowStartupLocationProperty,
        "WindowStartupLocation", FrameworkPropertyMetadata::Create(WindowStartupLocation_Manual));

    data->OverrideMetadata<Ptr<Brush>>(Control::BackgroundProperty, "Background",
        FrameworkPropertyMetadata::Create(CoerceValueCallback(CoerceBackground)));

    data->OverrideMetadata<bool>(Control::IsTabStopProperty, "IsTabStop",
        FrameworkPropertyMetadata::Create(false));
    data->OverrideMetadata<KeyboardNavigationMode>(KeyboardNavigation::DirectionalNavigationProperty,
        "DirectionalNavigation", FrameworkPropertyMetadata::Create(KeyboardNavigationMode_Cycle));
    data->OverrideMetadata<KeyboardNavigationMode>(KeyboardNavigation::TabNavigationProperty,
        "TabNavigation", FrameworkPropertyMetadata::Create(KeyboardNavigationMode_Cycle));
    data->OverrideMetadata<KeyboardNavigationMode>(KeyboardNavigation::ControlTabNavigationProperty,
        "ControlTabNavigation", FrameworkPropertyMetadata::Create(KeyboardNavigationMode_Cycle));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const DependencyProperty* Window::AllowsTransparencyProperty;
const DependencyProperty* Window::FullscreenProperty;
const DependencyProperty* Window::IsActiveProperty;
const DependencyProperty* Window::LeftProperty;
const DependencyProperty* Window::ResizeModeProperty;
const DependencyProperty* Window::ShowInTaskbarProperty;
const DependencyProperty* Window::TitleProperty;
const DependencyProperty* Window::TopmostProperty;
const DependencyProperty* Window::TopProperty;
const DependencyProperty* Window::WindowStateProperty;
const DependencyProperty* Window::WindowStyleProperty;
const DependencyProperty* Window::WindowStartupLocationProperty;
