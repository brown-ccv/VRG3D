//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

#include  "VRApp.h"
#include  "ConfigMap.h"

using namespace G3D;
using namespace std;


namespace VRG3D {


void  VRApp::initScreenSize( GWindow::Settings  &gwinsettings )
{     Display * disp;
      int screen;
      disp=XOpenDisplay(NULL);
      screen = DefaultScreen(disp);
      gwinsettings.width = DisplayWidth(disp, screen);
      gwinsettings.height = DisplayHeight(disp, screen);
      XCloseDisplay(disp);

      /*
      // This is a bit hacky.. trying to figure out the max (best) screen resolution.
      // The problem is we need to start SDL in order to query the resolution.  So, 
      // we create a temporary SDL window here, query the resolution, and later create
      // our rendering window using this resolution.  In recent versions of SDL asking
      // for a window of size 0 x 0 gives us SDL's default "best" resolution.
      gwinsettings.width = 0;
      gwinsettings.height = 0;
      OSWindow *tmpWin = OSWindow::create(gwinsettings);
      //const SDL_VideoInfo *vinfo = SDL_GetVideoInfo();
      gwinsettings.width = 1024;
      gwinsettings.height = 768;
      //delete tmpWin;
      _log->println("Max display dimensions reported by SDL: " + intToString(gwinsettings.width) 
                    + " x " + intToString(gwinsettings.height));
      */
}


void  VRApp::initCreateWindow( GWindow::Settings  &gwinsettings )
{
     // Linux here..  use SDLWindow as compiled as part of G3D since
     // this is still G3D's default for linux environments.
     _gwindow = new G3D::SDLWindow(gwinsettings);
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
