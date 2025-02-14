//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

/**
 *
 * Note: Copied CarbonWindow from G3D.. making some bug fixes and calling it OSXWindow
 *
  @file OSXWindow.h

  @maintainer Casey O'Donnell, caseyodonnell@gmail.com
  @created 2006-08-20
  @edited  2007-08-23
*/

#ifndef VRG3D_OSXWINDOW_H
#define VRG3D_OSXWINDOW_H

#include <G3D/platform.h>
#include <G3D/Set.h>
#include <G3D/Rect2D.h>

#ifdef G3D_OSX

///#include <GLG3D/GWindow.h>
#include <GLG3D/OSWindow.h>
#include <GLG3D/glcalls.h>

#include <Carbon/Carbon.h>
#include <CoreFoundation/CoreFoundation.h>

#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/IOKitLib.h>
#include <Kernel/IOKit/hidsystem/IOHIDUsageTables.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/IOCFPlugIn.h>

#include <OpenGL/Opengl.h>
#include <OpenGL/CGLTypes.h>
#include <AGL/agl.h>

using namespace G3D;

namespace VRG3D {

namespace _internal {
pascal OSStatus OnWindowSized(EventHandlerCallRef handlerRef, EventRef event, void *userData);
pascal OSStatus OnWindowClosed(EventHandlerCallRef handlerRef, EventRef event, void *userData);
pascal OSStatus OnAppQuit(EventHandlerCallRef handlerRef, EventRef event, void *userData);
pascal OSStatus OnActivation(EventHandlerCallRef handlerRef, EventRef event, void *userData);
pascal OSStatus OnDeactivation(EventHandlerCallRef handlerRef, EventRef event, void *userData);
pascal OSStatus OnDeviceScroll(EventHandlerCallRef handlerRef, EventRef event, void *userData);
pascal OSErr OnDragReceived(WindowRef theWindow, void *userData, DragRef theDrag);
void HIDCollectJoyElementsArrayHandler(const void *value, void *parameter);
}

class OSXWindow : public GWindow {
private:
    // Window Settings
    Vector2                 _clientRectOffset;
    Vector2 _clientXY;
    Settings                _settings;
    std::string                _title;

    /** Modifiers down on last key event */
    GKeyMod lastMod;

    // Process to Front Initialization
    static bool                 _ProcessBroughtToFront;

    // State Information
    bool                        _mouseVisible;
    bool    _inputCapture;
    bool    _windowActive;

    bool    _receivedCloseEvent;

    //  Mouse Button State Array: false - up, true - down
    //[0] - left, [1] - middle, [2] - right, [3] - X1,  [4] - X2
    bool    _mouseButtons[8];
    bool    _keyboardButtons[256];

    // Joystick Classes
    class GJoyElement {
    public:
        IOHIDElementCookie cookie;  // unique value which identifies element, will NOT change
        long min;   // reported min value possible
        long max;   // reported max value possible

        long value;                 // actual value of element

        // runtime variables used for auto-calibration
        long minReport;// min returned value
        long maxReport;// max returned value
    };

    class GJoyDevice {
    public:
        IOHIDDeviceInterface **interface;   // interface to device, NULL = no interface

        std::string product;// name of product
        long usage; // usage page from IOUSBHID Parser.h which defines general usage
        long usagePage;// usage within above page from IOUSBHID Parser.h which defines specific usage

        Array<GJoyElement> axis;
        Array<GJoyElement> button;
        Array<GJoyElement> hat;

        int removed;
        int uncentered;

        bool buildDevice(io_object_t hidDevice);
        void addJoyElement(CFTypeRef refElement);
    };

    // TODO: Add Input Device Array
    bool                   _enabledJoysticks;
    Array<GJoyDevice>   _joysticks;

    // Carbon Window Data
    WindowRef _window;
    AGLContext_glContext;
    AGLDrawable_glDrawable;

    const bool_createdWindow;

    // Make Event Handlers Capable of Seeing Private Parts
    friend pascal OSStatus _internal::OnWindowSized(EventHandlerCallRef handlerRef, EventRef event, void *userData);
    friend pascal OSStatus _internal::OnWindowClosed(EventHandlerCallRef handlerRef, EventRef event, void *userData);
    friend pascal OSStatus _internal::OnAppQuit(EventHandlerCallRef handlerRef, EventRef event, void *userData);
    friend pascal OSStatus _internal::OnActivation(EventHandlerCallRef handlerRef, EventRef event, void *userData);
    friend pascal OSStatus _internal::OnDeactivation(EventHandlerCallRef handlerRef, EventRef event, void *userData);
    friend pascal OSStatus _internal::OnDeviceScroll(EventHandlerCallRef handlerRef, EventRef event, void *userData);
    friend pascal OSErr _internal::OnDragReceived(WindowRef theWindow, void *userData, DragRef theDrag);
    friend void _internal::HIDCollectJoyElementsArrayHandler(const void *value, void *parameter);

    static EventTypeSpec _resizeSpec[];
    static EventTypeSpec _closeSpec[];
    static EventTypeSpec _appQuitSpec[];
    static EventTypeSpec _aeSpec[];
    static EventTypeSpec _activateSpec[];
    static EventTypeSpec _deactivateSpec[];
    static EventTypeSpec _deviceScrollSpec[];

    Array<GEvent>   _sizeEventInjects;
    Array<std::string>_droppedFiles;

    void injectSizeEvent(int width, int height) {
        GEvent e;
        e.type = GEventType::VIDEO_RESIZE;
        e.resize.w = width;
        e.resize.h = height;
        _sizeEventInjects.append(e);
    }

    bool makeMouseEvent(EventRef theEvent, GEvent& e);

    void findJoysticks(UInt32 usagePage, UInt32 usage);
    bool enableJoysticks();

    // Called from all constructors
    void init(WindowRef window, bool creatingShareWindow = false);

    static std::auto_ptr<OSXWindow> _shareWindow;

    static void createShareWindow(GWindow::Settings s);

    // Constructs from a new window
    explicit OSXWindow(const GWindow::Settings& settings, bool creatingShareWindow = false);

    // Constructs from an existing window
    explicit OSXWindow(const GWindow::Settings& settings, WindowRef window);

    OSXWindow& operator=(const OSXWindow& other);

    /** Fills out @e and returns the index of the key for use with _keyboardButtons.*/
    unsigned char makeKeyEvent(EventRef theEvent, GEvent& e);

public:

    static OSXWindow* create(const GWindow::Settings& settings = GWindow::Settings());

    static OSXWindow* create(const GWindow::Settings& settings, WindowRef window);

    virtual ~OSXWindow();

    std::string getAPIVersion() const;
    std::string getAPIName() const;

    // The WindowRef of this Object
    inline  WindowRef windowref() const {
        return _window;
    }

    virtual void getSettings(GWindow::Settings& settings) const;

    virtual int width() const;
    virtual int height() const;

    virtual Rect2D dimensions() const;
    virtual void setDimensions(const Rect2D &dims);

    virtual void getDroppedFilenames(Array<std::string>& files);

    virtual void setPosition(int x, int y) {
        setDimensions( Rect2D::xywh((float)x, (float)y, (float)_settings.width, (float)_settings.height) );
    }

    virtual bool hasFocus() const;

    virtual void setGammaRamp(const Array<G3D::uint16>& gammaRamp);

    virtual void setCaption(const std::string& title);
    virtual std::string caption() ;

    virtual int numJoysticks() const;

    virtual std::string joystickName(unsigned int stickNum);

    virtual void setIcon(const GImage& image);

    virtual void notifyResize(int w, int h);

    virtual void setRelativeMousePosition(double x, double y);
    virtual void setRelativeMousePosition(const Vector2 &p);
    virtual void getRelativeMouseState(Vector2 &position, G3D::uint8 &mouseButtons) const;
    virtual void getRelativeMouseState(int &x, int &y, G3D::uint8 &mouseButtons) const;
    virtual void getRelativeMouseState(double &x, double &y, G3D::uint8 &mouseButtons) const;
    virtual void getJoystickState(unsigned int stickNum, Array<float> &axis, Array<bool> &buttons);

    virtual void setInputCapture(bool c);
    virtual bool inputCapture() const;

    virtual void setMouseVisible(bool b);
    virtual bool mouseVisible() const;

    virtual bool requiresMainLoop() const {
        return false;
    }

    virtual void swapGLBuffers();

    virtual bool processSystemEvent(EventRef theEvent, OSStatus osErr, GEvent &e);

protected:

    virtual void reallyMakeCurrent() const;

    virtual bool pollOSEvent(GEvent& e);


}; // OSXWindow

} // namespace G3D


#endif // G3D_OSX

#endif // G3D_OSXWindow_H
