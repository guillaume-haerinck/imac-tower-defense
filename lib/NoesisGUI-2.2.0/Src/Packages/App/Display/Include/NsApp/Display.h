////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
// [CR #1020]
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_DISPLAY_H__
#define __APP_DISPLAY_H__


#include <NsCore/Noesis.h>
#include <NsApp/DisplayApi.h>
#include <NsGui/Enums.h>
#include <NsGui/InputEnums.h>
#include <NsCore/BaseComponent.h>
#include <NsCore/Delegate.h>
#include <NsCore/ReflectionDeclare.h>
#include <NsCore/ReflectionDeclareEnum.h>


namespace Noesis
{
class UIElement;
}

namespace NoesisApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Specifies whether a window can be resized and, if so, how it can be resized
////////////////////////////////////////////////////////////////////////////////////////////////////
enum ResizeMode
{
    /// A window can only be minimized and restored. The Minimize and Maximize buttons are both
    /// shown, but only the Minimize button is enabled
    ResizeMode_CanMinimize,

    /// A window can be resized. The Minimize and Maximize buttons are both shown and enabled
    ResizeMode_CanResize,

    /// A window can be resized. The Minimize and Maximize buttons are both shown and enabled.
    /// A resize grip appears in the bottom-right corner of the window
    ResizeMode_CanResizeWithGrip,

    /// A window cannot be resized. The Minimize and Maximize buttons are not displayed in the
    /// title bar
    ResizeMode_NoResize
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Specifies whether a window is minimized, maximized, or restored
////////////////////////////////////////////////////////////////////////////////////////////////////
enum WindowState
{
    /// The window is maximized
    WindowState_Maximized,

    /// The window is minimized
    WindowState_Minimized,

    /// The window is restored
    WindowState_Normal 

};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Specifies the type of border that a Window has
////////////////////////////////////////////////////////////////////////////////////////////////////
enum WindowStyle
{
    /// Only the client area is visible - the title bar and border are not shown
    WindowStyle_None,

    /// A window with a single border. This is the default value
    WindowStyle_SingleBorderWindow,

    /// A window with a 3-D border
    WindowStyle_ThreeDBorderWindow,

    /// A fixed tool window
    WindowStyle_ToolWindow 
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Specifies the position that a Window will be shown in when it is first opened
////////////////////////////////////////////////////////////////////////////////////////////////////
enum WindowStartupLocation
{
    /// The startup location of a Window is set from code, or defers to the default location
    WindowStartupLocation_Manual,

    /// The startup location of a Window is the center of the screen that contains the mouse cursor
    WindowStartupLocation_CenterScreen,

    /// The startup location of a Window is the center of the Window that owns it, as specified by
    /// the Window Owner property
    WindowStartupLocation_CenterOwner
};

NS_WARNING_PUSH
NS_MSVC_WARNING_DISABLE(4251)

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Abstraction for operating system windows 
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_APP_DISPLAY_API Display: public Noesis::BaseComponent
{
public:
    Display();
    ~Display();

    /// Stores custom private data
    static void SetPrivateData(void* data);

    /// Returns stored private data
    static void* GetPrivateData();

    /// Changes the text of the title bar (if it has one)
    virtual void SetTitle(const char* title);

    /// Changes position
    virtual void SetLocation(int x, int y);

    /// Changes size
    virtual void SetSize(uint32_t width, uint32_t height);

    /// Sets window border style
    virtual void SetWindowStyle(WindowStyle windowStyle);

    /// Sets window state
    virtual void SetWindowState(WindowState windowState);

    /// Sets window resize mode
    virtual void SetResizeMode(ResizeMode resizeMode);

    /// Sets if the window has a task bar button 
    virtual void SetShowInTaskbar(bool showInTaskbar);

    /// Sets if a window appears in the topmost z-order
    virtual void SetTopmost(bool topmost);

    /// Sets if this element can be used as the target of a drag-and-drop operation
    virtual void SetAllowFileDrop(bool allowFileDrop);

    /// Sets the position of the window when first shown
    virtual void SetWindowStartupLocation(WindowStartupLocation location);

    /// Activates the window and displays it in its current size and position
    virtual void Show();

    /// Starts application message loop, returning when application is shutting down
    virtual void EnterMessageLoop(bool runInBackground) = 0;

    /// Destroys window
    virtual void Close();

    /// Opens on-screen keyboard
    virtual void OpenSoftwareKeyboard(Noesis::UIElement* focused);

    /// Closes on-screen keyboard
    virtual void CloseSoftwareKeyboard();

    /// Updates mouse cursor icon
    virtual void SetCursor(Noesis::Cursor cursor);

    /// Opens URL in a browser
    virtual void OpenUrl(const char* url);

    /// Gets system window handle
    virtual void* GetNativeHandle() const = 0;

    /// Returns the width of the window client area
    virtual uint32_t GetClientWidth() const = 0;

    /// Returns the height of the window client area
    virtual uint32_t GetClientHeight() const = 0;

    /// Plays audio file
    void PlaySound(const char* filename, float volume);
    
    /// Halts the currently playing audio
    void PauseAudio();

    /// Resumes playback
    void ResumeAudio();

    /// Occurs when the window's location changes
    typedef void LocationChangedT(Display* display, int x, int y);
    Noesis::Delegate<LocationChangedT>& LocationChanged();

    /// Occurs when the window's size changes
    typedef void SizeChangedT(Display* display, uint32_t width, uint32_t height);
    Noesis::Delegate<SizeChangedT>& SizeChanged();

    /// Occurs when the window's state changes
    typedef void StateChangedT(Display* display, WindowState state);
    Noesis::Delegate<StateChangedT>& StateChanged();

    /// Occurs when files are dragged and dropped over the window
    typedef void FileDroppedT(Display* display, const char* filename);
    Noesis::Delegate<FileDroppedT>& FileDropped();

    /// Occurs when a window becomes the foreground window
    typedef void ActivatedT(Display* display);
    Noesis::Delegate<ActivatedT>& Activated();

    /// Occurs when a window becomes a background window
    typedef void DeactivatedT(Display* display);
    Noesis::Delegate<DeactivatedT>& Deactivated();

    /// Occurs when window needs to render a frame
    typedef void RenderT(Display* display);
    Noesis::Delegate<RenderT>& Render();

    /// Occurs when mouse is moved over the window
    typedef void MouseMoveT(Display* display, int x, int y);
    Noesis::Delegate<MouseMoveT>& MouseMove();

    /// Occurs when a mouse button is pressed over the window
    typedef void MouseButtonDownT(Display* display, int x, int y, Noesis::MouseButton b);
    Noesis::Delegate<MouseButtonDownT>& MouseButtonDown();

    /// Occurs when a mouse button is released over the window
    typedef void MouseButtonUpT(Display* display, int x, int y, Noesis::MouseButton b);
    Noesis::Delegate<MouseButtonUpT>& MouseButtonUp();

    /// Occurs when a mouse button is double clicked over the window
    typedef void MouseDoubleClickT(Display* display, int x, int y, Noesis::MouseButton b);
    Noesis::Delegate<MouseDoubleClickT>& MouseDoubleClick();

    /// Occurs when mouse vertical wheel is rotated over the window
    typedef void MouseWheelT(Display* display, int x, int y, int delta);
    Noesis::Delegate<MouseWheelT>& MouseWheel();

    /// Occurs when mouse horizontal wheel is rotated over the window
    typedef void MouseHWheelT(Display* display, int x, int y, int delta);
    Noesis::Delegate<MouseHWheelT>& MouseHWheel();

    /// Occurs when vertical scroll axis is actioned
    typedef void ScrollT(Display* display, float value);
    Noesis::Delegate<ScrollT>& Scroll();

    /// Occurs when horizontal scroll axis is actioned
    typedef void HScrollT(Display* display, float value);
    Noesis::Delegate<HScrollT>& HScroll();

    /// Occurs when a key is pressed over the window
    typedef void KeyDownT(Display* display, Noesis::Key key);
    Noesis::Delegate<KeyDownT>& KeyDown();

    /// Occurs when a key is released over the window
    typedef void KeyUpT(Display* display, Noesis::Key key);
    Noesis::Delegate<KeyUpT>& KeyUp();

    /// Occurs when a key is translated to the corresponding character over the window
    typedef void CharT(Display* display, uint32_t c);
    Noesis::Delegate<CharT>& Char();

    /// Occurs when a finger touches the window
    typedef void TouchDownT(Display* display, int x, int y, uint64_t id);
    Noesis::Delegate<TouchDownT>& TouchDown();

    /// Occurs when a finger moves on the window
    typedef void TouchMoveT(Display* display, int x, int y, uint64_t id);
    Noesis::Delegate<TouchMoveT>& TouchMove();

    /// Occurs when a finger raises off of the window
    typedef void TouchUpT(Display* display, int x, int y, uint64_t id);
    Noesis::Delegate<TouchUpT>& TouchUp();

    /// Occurs when the native handle changes
    typedef void NativeHandleChangedT(Display* display, void* handle);
    Noesis::Delegate<NativeHandleChangedT>& NativeHandleChanged();

protected:
    Noesis::Delegate<LocationChangedT> mLocationChanged;
    Noesis::Delegate<SizeChangedT> mSizeChanged;
    Noesis::Delegate<StateChangedT> mStateChanged;
    Noesis::Delegate<FileDroppedT> mFileDropped;
    Noesis::Delegate<ActivatedT> mActivated;
    Noesis::Delegate<DeactivatedT> mDeactivated;
    Noesis::Delegate<RenderT> mRender;
    Noesis::Delegate<MouseMoveT> mMouseMove;
    Noesis::Delegate<MouseButtonDownT> mMouseButtonDown;
    Noesis::Delegate<MouseButtonUpT> mMouseButtonUp;
    Noesis::Delegate<MouseDoubleClickT> mMouseDoubleClick;
    Noesis::Delegate<MouseWheelT> mMouseWheel;
    Noesis::Delegate<MouseHWheelT> mMouseHWheel;
    Noesis::Delegate<ScrollT> mScroll;
    Noesis::Delegate<HScrollT> mHScroll;
    Noesis::Delegate<KeyDownT> mKeyDown;
    Noesis::Delegate<KeyUpT> mKeyUp;
    Noesis::Delegate<CharT> mChar;
    Noesis::Delegate<TouchDownT> mTouchDown;
    Noesis::Delegate<TouchMoveT> mTouchMove;
    Noesis::Delegate<TouchUpT> mTouchUp;
    Noesis::Delegate<NativeHandleChangedT> mNativeHandleChanged;

private:
    NS_DECLARE_REFLECTION(Display, BaseComponent)
};

NS_WARNING_POP

/// Creates a ISystemWindow choosing the best implementation available
NS_APP_DISPLAY_API Noesis::Ptr<Display> CreateDisplay();

}

NS_DECLARE_REFLECTION_ENUM_EXPORT(NS_APP_DISPLAY_API, NoesisApp::ResizeMode)
NS_DECLARE_REFLECTION_ENUM_EXPORT(NS_APP_DISPLAY_API, NoesisApp::WindowState)
NS_DECLARE_REFLECTION_ENUM_EXPORT(NS_APP_DISPLAY_API, NoesisApp::WindowStyle)
NS_DECLARE_REFLECTION_ENUM_EXPORT(NS_APP_DISPLAY_API, NoesisApp::WindowStartupLocation)


#endif
