//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


// Note: This include ordering is important, don't screw with it!
#include "VRPNTrackerDevice.h"
#include "Event.h"

#include <vrpn_Connection.h>
#include <vrpn_Tracker.h>


#include <iostream>
using namespace std;
using namespace G3D;

namespace VRG3D {


#ifndef VRPN_CALLBACK
#define VRPN_CALLBACK
#endif


// Callback function for VRPN, void* pointer points to a VRPNTrackerDevice
void VRPN_CALLBACK
trackerHandler(void *thisPtr, const vrpn_TRACKERCB info)
{
  CoordinateFrame vrpnEvent;
  vrpnEvent.rotation = Quat(info.quat[0],info.quat[1],
                            info.quat[2],info.quat[3]).toRotationMatrix();
  vrpnEvent.rotation.orthonormalize();
  vrpnEvent.translation = Vector3(info.pos[0],info.pos[1],info.pos[2]);

  VRPNTrackerDevice* device = ((VRPNTrackerDevice*)thisPtr);
  device->processEvent(vrpnEvent, info.sensor);
}



VRPNTrackerDevice::VRPNTrackerDevice(
    const std::string            &vrpnTrackerDeviceName,
    const Array<std::string>     &eventsToGenerate,
    const double                 &trackerUnitsToRoomUnitsScale,
    const CoordinateFrame        &deviceToRoom,
    const Array<CoordinateFrame> &propToTracker,
    const Array<CoordinateFrame> &finalOffset,
    const bool                   &waitForNewReportInPoll,
    const bool                   &convertLHtoRH,
    const bool                   &ignoreZeroes)
{
  _eventNames                   = eventsToGenerate;
  _trackerUnitsToRoomUnitsScale = trackerUnitsToRoomUnitsScale;
  _deviceToRoom                 = deviceToRoom;
  _propToTracker                = propToTracker;
  _finalOffset                  = finalOffset;
  _waitForNewReport             = waitForNewReportInPoll;
  _convertLHtoRH                = convertLHtoRH;
  _ignoreZeroes                 = ignoreZeroes;
  _printSensor0                 = false;

  _vrpnConnection = vrpn_get_connection_by_name(vrpnTrackerDeviceName.c_str());
  _vrpnDevice = new vrpn_Tracker_Remote(vrpnTrackerDeviceName.c_str(), _vrpnConnection);
  alwaysAssertM(_vrpnDevice,
     "Can't create VRPN Remote Tracker with name " + vrpnTrackerDeviceName);
  _vrpnDevice->register_change_handler(this, trackerHandler);
}



VRPNTrackerDevice::VRPNTrackerDevice( const std::string   name,
                                                 Log     *log,
                                      const ConfigMapRef  map )
{
  std::string vrpnname  = map->get( name + "_VRPNTrackerDeviceName", "" );
  std::string eventsStr = map->get( name + "_EventsToGenerate", "" );
  Array<std::string> events = splitStringIntoArray(eventsStr);

  double scale = map->get( name + "_TrackerUnitsToRoomUnitsScale", 1.0 );
  CoordinateFrame d2r = map->get( name + "_DeviceToRoom", CoordinateFrame() );
  d2r.rotation.orthonormalize();

  Array<CoordinateFrame> p2t;
  Array<CoordinateFrame> fo;

  for (int  i = 0; i < events.size(); i++)
  { CoordinateFrame cf = map->get(events[i] + "_PropToTracker", CoordinateFrame());
    cf.rotation.orthonormalize();
    p2t.append(cf);
    CoordinateFrame cf2 = map->get(events[i] + "_FinalOffset", CoordinateFrame());
    cf2.rotation.orthonormalize();
    fo.append(cf2);
  }

  bool wait          = map->get( name + "_WaitForNewReportInPoll", false );
  bool convertLHtoRH = map->get( name + "_ConvertLHtoRH", false );
  bool ignoreZeroes  = map->get( name + "_IgnoreZeroes", false );

  log->println( "Creating new VRPNTrackerDevice ( " + vrpnname + ")" );

  _eventNames                   = events;
  _trackerUnitsToRoomUnitsScale = scale;
  _deviceToRoom                 = d2r;
  _propToTracker                = p2t;
  _finalOffset                  = fo;
  _waitForNewReport             = wait;
  _convertLHtoRH                = convertLHtoRH;
  _ignoreZeroes                 = ignoreZeroes;
  _printSensor0                 = false;

  _vrpnConnection = vrpn_get_connection_by_name(vrpnname.c_str());
  _vrpnDevice = new vrpn_Tracker_Remote(vrpnname.c_str(), _vrpnConnection);
  alwaysAssertM(_vrpnDevice,
     "Can't create VRPN Remote Tracker with name " + vrpnname);
  _vrpnDevice->register_change_handler(this, trackerHandler);
}



VRPNTrackerDevice::~VRPNTrackerDevice()
{}


/**
   The tracker reports the transformation that would transform stuff
   in tracker space to device space (that is, to the space relative to
   the origin of the tracking device).  First we change the units on
   the translation part of this since we usually work in feet and most
   tracking systems report mesurements in some metric system.  Then we
   have a transformation in feet relative to the device.  We multiply
   by a transformation that transforms stuff in device space to IS3D
   room space.  You can think of this as what rotation, then
   translation would move the origin of RoomSpace to the origin of
   tracking device.  This is the deviceToRoom coordinate frame.
 */
void
VRPNTrackerDevice::processEvent(const CoordinateFrame &vrpnEvent, int sensorNum)
{
  if(_ignoreZeroes && vrpnEvent.translation == Vector3::zero()){
    return;
  }
  _newReportFlag = true;

  // first, adjust units of trackerToDevice.  after this, everything
  // is in RoomSpace units (typically feet for VRG3D).
  CoordinateFrame trackerToDevice = vrpnEvent;

  // convert a left handed coordinate system to a right handed one
  // not sure if this actually works..
  if (_convertLHtoRH) {
    /*** This is code from syzygy..
    CoordinateFrame switchMat(Matrix3(1,0,0, 0,0,1, 0,-1,0), Vector3::zero());
    CoordinateFrame transMat(switchMat.inverse() * vrpnEvent.translation);
    CoordinateFrame rotMat(vrpnEvent.rotation, Vector3::zero());
    trackerToDevice = transMat * switchMat.inverse() * rotMat * switchMat;
    ***/

    // This code is based on the article "Conversion of Left-Handed
    // Coordinates to Right-Handed Coordinates" by David Eberly,
    // available online:
    // http://www.geometrictools.com/Documentation/LeftHandedToRightHanded.pdf
    trackerToDevice.translation[2] = -trackerToDevice.translation[2];

    trackerToDevice.rotation[0][2] = -trackerToDevice.rotation[0][2];
    trackerToDevice.rotation[1][2] = -trackerToDevice.rotation[1][2];
    trackerToDevice.rotation[2][0] = -trackerToDevice.rotation[2][0];
    trackerToDevice.rotation[2][1] = -trackerToDevice.rotation[2][1];
  }

  trackerToDevice.translation *= _trackerUnitsToRoomUnitsScale;

  CoordinateFrame eventRoom = _finalOffset[sensorNum] * _deviceToRoom * trackerToDevice * _propToTracker[sensorNum];

  if ((_printSensor0) && (sensorNum == 0)) {
    std::cout << eventRoom.translation << std::endl;
  }
  _pendingEvents.append(new Event(getEventName(sensorNum), eventRoom));
}


std::string
VRPNTrackerDevice::getEventName(int trackerNumber)
{
  if (trackerNumber >= _eventNames.size())
    return std::string("VRPNTrackerDevice_Unknown_Event");
  else
    return _eventNames[trackerNumber];
}

void
VRPNTrackerDevice::pollForInput(Array<EventRef> &events)
{
  // If this poll routine isn't called fast enough then the UDP buffer can fill up and
  // the most recent tracker records will be dropped, introducing lag in the system.
  // A workaround suggested by the VRPN website is to keep calling mainloop() until you
  // get a new tracker report.  This should only really be an issue if your framerate
  // is low.
  if (_waitForNewReport) {
    _newReportFlag = false;
    while (!_newReportFlag) {
      _vrpnDevice->mainloop();
      _vrpnConnection->mainloop();
    }
  }
  else {
    _vrpnDevice->mainloop();
    _vrpnConnection->mainloop();
  }

  events.append(_pendingEvents);
  _pendingEvents.clear();
}


} // end namespace
