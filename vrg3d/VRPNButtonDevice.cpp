//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


// Note: This include ordering is important, don't screw with it!
#include "VRPNButtonDevice.h"
#include "Event.h"
#include <vrpn_Button.h>


#ifndef VRPN_CALLBACK
#define VRPN_CALLBACK
#endif

namespace VRG3D {

using namespace G3D;

void  VRPN_CALLBACK  buttonHandler(void *thisPtr, const vrpn_BUTTONCB info)
{
  ((VRPNButtonDevice*)thisPtr)->sendEvent(info.button, info.state);
}



VRPNButtonDevice::VRPNButtonDevice( const  std::string &vrpnButtonDeviceName,
                                    const  Array<std::string> &eventsToGenerate )

{
  _eventNames = eventsToGenerate;

  _vrpnDevice = new vrpn_Button_Remote(vrpnButtonDeviceName.c_str());
  if (!_vrpnDevice) {
    alwaysAssertM(false, "Can't create VRPN Remote Button with name " +
      vrpnButtonDeviceName);
  }

  _vrpnDevice->register_change_handler(this, buttonHandler);
}



VRPNButtonDevice::VRPNButtonDevice( const std::string   name,
                                               Log     *log,
                                    const ConfigMapRef  map )
{
  std::string vrpnname = map->get( name + "_VRPNButtonDeviceName", "" );
  std::string events   = map->get( name + "_EventsToGenerate","" );

  log->println("Creating new VRPNButtonDevice (" + vrpnname + ")");

  _eventNames = splitStringIntoArray( events );

  _vrpnDevice = new vrpn_Button_Remote(vrpnname.c_str());
  if (!_vrpnDevice) {
    alwaysAssertM(false, "Can't create VRPN Remote Button with name " +
      vrpnname);
  }

  _vrpnDevice->register_change_handler(this, buttonHandler);
}

 

VRPNButtonDevice::~VRPNButtonDevice()
{}



std::string  VRPNButtonDevice::getEventName(int buttonNumber)
{
  if (buttonNumber >= _eventNames.size())
  { return std::string("VRPNButtonDevice_Unknown_Event");
  }

  return  _eventNames[buttonNumber];
}

void  VRPNButtonDevice::sendEvent( int buttonNumber, bool down )
{
  std::string ename = getEventName(buttonNumber);

  if (down)
  { _pendingEvents.append(new Event(ename + "_down"));
  }
  else
  { _pendingEvents.append(new Event(ename + "_up"));
  }
}



void  VRPNButtonDevice::pollForInput(Array<EventRef> &events)
{
  _vrpnDevice->mainloop();

  if (_pendingEvents.size())
  { events.append(_pendingEvents);
    _pendingEvents.clear();
  }
}


}            // end namespace
