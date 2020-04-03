//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

// Note: This include ordering is important, don't screw with it!
#include "VRPNAnalogDevice.h"
#include "Event.h"
#include <vrpn_Analog.h>


#ifndef VRPN_CALLBACK
#define VRPN_CALLBACK
#endif

namespace VRG3D {

using namespace G3D;

void VRPN_CALLBACK  analogHandler( void *thisPtr, const vrpn_ANALOGCB info )
{
  int lastchannel = (int)G3D::min(info.num_channel, ((VRPNAnalogDevice*)thisPtr)->numChannels());

  for (int i=0;i<lastchannel;i++)
  {
    ((VRPNAnalogDevice*)thisPtr)->sendEventIfChanged(i, info.channel[i]);
  }
}



VRPNAnalogDevice::VRPNAnalogDevice(const std::string &vrpnAnalogDeviceName,
                                   const Array<std::string> &eventsToGenerate)
{
  _eventNames = eventsToGenerate;
  for (int i=0;i<_eventNames.size();i++)
  { _channelValues.append(0.0);
  }

  _vrpnDevice = new vrpn_Analog_Remote(vrpnAnalogDeviceName.c_str());

  if (!_vrpnDevice)
  { alwaysAssertM(false, "Can't create VRPN Remote Analog with name" +
    vrpnAnalogDeviceName);
  }

  _vrpnDevice->register_change_handler(this, analogHandler);
}



VRPNAnalogDevice::VRPNAnalogDevice( const std::string   name,
                                               Log     *log,
                                    const ConfigMapRef  map )
{
  std::string  vrpnname = map->get( name + "_VRPNAnalogDeviceName", "" );
  std::string  events   = map->get( name + "_EventsToGenerate", "" );

  log->println( "Creating new VRPNAnalogDevice (" + vrpnname + ")" );

  _eventNames = splitStringIntoArray( events );
  for (int i=0;i<_eventNames.size();i++)
  { _channelValues.append(0.0);
  }

  _vrpnDevice = new vrpn_Analog_Remote(vrpnname.c_str());

  if (!_vrpnDevice)
  { alwaysAssertM(false, "Can't create VRPN Remote Analog with name" +
    vrpnname);
  }

  _vrpnDevice->register_change_handler(this, analogHandler);
}



VRPNAnalogDevice::~VRPNAnalogDevice()
{}


std::string  VRPNAnalogDevice::getEventName(int channelNumber)
{
  if (channelNumber >= _eventNames.size())
  { return std::string("VRPNAnalogDevice_Unknown_Event");
  }

  return _eventNames[channelNumber];
}



void  VRPNAnalogDevice::sendEventIfChanged(int channelNumber, double data)
{
  if (_channelValues[channelNumber] != data)
  { _pendingEvents.append(new Event(_eventNames[channelNumber], data));
    _channelValues[channelNumber] = data;
  }
}



void  VRPNAnalogDevice::pollForInput(Array<EventRef> &events)
{
  _vrpnDevice->mainloop();

  if (_pendingEvents.size())
  { events.append(_pendingEvents);
    _pendingEvents.clear();
  }
}


}                      // end namespace VRG3D
