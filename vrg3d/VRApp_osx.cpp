//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

#include  "VRApp.h"
#include  "ConfigMap.h"

using namespace G3D;
using namespace std;


namespace VRG3D {


void  VRApp::initScreenSize( GWindow::Settings  &gwinsettings )
{
      CGRect mainMonitor = CGDisplayBounds(CGMainDisplayID());
      CGFloat monitorHeight = CGRectGetHeight(mainMonitor);
      CGFloat monitorWidth = CGRectGetWidth(mainMonitor);

      gwinsettings.width = monitorWidth;
      gwinsettings.height = monitorHeight;
}



void  VRApp::initCreateWindow( GWindow::Settings  &gwinsettings )
{
     /** We're now using our own version of G3D::CarbonWindow recompiled in VRG3D and
      * renamed OSXWindow.  It fixes some bugs and extends functionality to allow for
      * multiple rendering windows.  In the past, we also tried our own version of
      * SDLWindow, but it couldn't open multiple windows correctly on OSX.
      **/

     //_gwindow = new VRG3D::SDLWindow(gwinsettings);
     //_gwindow = CarbonWindow::create(gwinsettings);
    cerr << "VRG3D on OS X only supported with USE_GLUT." << endl;
    abort();
    //_gwindow = OSXWindow::create(gwinsettings);
}


void vrg3dSleepMsecs(double msecs)
{
  // Routine grabbed from VRPN's vrpn_SleepMsecs routine

  timeval timeout;
  // Convert milliseconds to seconds
  timeout.tv_sec = (int)(msecs / 1000);
  // Subtract of whole number of seconds
  msecs -= timeout.tv_sec * 1000;
  // Convert remaining milliseconds to microsec
  timeout.tv_usec = (int)(msecs * 1000);
  // A select() with NULL file descriptors acts like a microsecond timer.
  select(0, 0, 0, 0, &timeout);
}


} // end namespace
