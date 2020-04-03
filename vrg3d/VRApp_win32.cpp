//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

#include  "VRApp.h"
#include  "CheckerboardStencil.h"
#include  "HorizontalStencil.h"
#include  "ConfigMap.h"
#include  "EventNet.h"
#include  "SynchedSystem.h"


// devices
//#include "ICubeXDevice.h"
//#include "ISenseDirect.h"
//#include "SpaceNavDevice.h"
//#include "TrackDClient.h"
//#include "VRPNAnalogDevice.h"
//#include "VRPNButtonDevice.h"
//#include "VRPNTrackerDevice.h"


using namespace std;
using namespace G3D;

namespace VRG3D {


void  VRApp::initScreenSize( GWindow::Settings  &gwinsettings )
{
      gwinsettings.width   = GetSystemMetrics(78);
      gwinsettings.height  = GetSystemMetrics(79);
      _log->println("Max display dimensions reported by Windows: " + intToString(gwinsettings.width)
                    + " x " + intToString(gwinsettings.height));
}



void  VRApp::initCreateWindow( GWindow::Settings  &gwinsettings )
{
      // Windows here.. Use G3D's native Win32Window class.
     _gwindow = Win32Window::create(gwinsettings);
}



void vrg3dSleepMsecs(double msecs)
{
  // Routine grabbed from VRPN's vrpn_SleepMsecs routine
  Sleep((DWORD)msecs);
}


} // end namespace
