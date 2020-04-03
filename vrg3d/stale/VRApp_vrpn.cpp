//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

#include  "VRApp.h"
#include  "CheckerboardStencil.h"
#include  "ConfigMap.h"
#include  "EventNet.h"
#include  "SynchedSystem.h"

#ifdef USE_GLUT
#  include  "GlutWindow.h"
#endif

#include  <SDL/SDL.h>

// devices
#include "ICubeXDevice.h"
#include "ISenseDirect.h"
#include "SpaceNavDevice.h"
#include "TrackDClient.h"

#ifdef USE_VRPN
   #include "VRPNAnalogDevice.h"
   #include "VRPNButtonDevice.h"
   #include "VRPNTrackerDevice.h"
#endif

#include <iostream>    //+++
using namespace std;


// if the program is a client and the server is running on the same machine, then
// it needs to sleep a bit to allow the server to have some cycles
#define CLIENT_SLEEP 0.0005


namespace VRG3D {

void  VRApp::setupInputDevicesFromConfigFile_vrpn(
          Log *log,  std::string type,
               std::string devname
{

    if (type == "VRPNAnalogDevice") {
      std::string vrpnname = map->get(devname + "_VRPNAnalogDeviceName","");
      std::string events = map->get(devname + "_EventsToGenerate","");
      log->println("Creating new VRPNAnalogDevice (" + vrpnname + ")");
      newDevice = new VRPNAnalogDevice(vrpnname, splitStringIntoArray(events));
    }
    else if (type == "VRPNButtonDevice") {
      std::string vrpnname = map->get(devname + "_VRPNButtonDeviceName","");
      std::string events = map->get(devname + "_EventsToGenerate","");
      log->println("Creating new VRPNButtonDevice (" + vrpnname + ")");
      newDevice = new VRPNButtonDevice(vrpnname, splitStringIntoArray(events));
    }
    else if (type == "VRPNTrackerDevice") {
      std::string vrpnname = map->get(devname + "_VRPNTrackerDeviceName","");
      std::string eventsStr = map->get(devname + "_EventsToGenerate","");
      Array<std::string> events = splitStringIntoArray(eventsStr);

      double scale = map->get(devname + "_TrackerUnitsToRoomUnitsScale", 1.0);
      CoordinateFrame d2r = map->get(devname + "_DeviceToRoom", CoordinateFrame());
      d2r.rotation.orthonormalize();

      Array<CoordinateFrame> p2t;
      Array<CoordinateFrame> fo;
      for (int i=0;i<events.size();i++) {
        CoordinateFrame cf = map->get(events[i] + "_PropToTracker", CoordinateFrame());
        cf.rotation.orthonormalize();
        p2t.append(cf);
        CoordinateFrame cf2 = map->get(events[i] + "_FinalOffset", CoordinateFrame());
        cf2.rotation.orthonormalize();
        fo.append(cf2);
      }

      bool wait = map->get(devname + "_WaitForNewReportInPoll", false);
      bool convertLHtoRH = map->get(devname + "_ConvertLHtoRH", false);
      bool ignoreZeroes = map->get(devname + "_IgnoreZeroes", false);

      log->println("Creating new VRPNTrackerDevice (" + vrpnname + ")");
      newDevice = new VRPNTrackerDevice(vrpnname, events, scale, d2r, p2t, fo, wait, convertLHtoRH, ignoreZeroes);
    }

} // end namespace

