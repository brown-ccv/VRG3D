//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

#include "Midi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace VRG3D {


MidiInDevice  *MidiInDevice::fromMidiDeviceName(const char *name)
{
  int numDevs = getNumAvailableDevices();
  int i=0;
  bool gotmatch = false;

  while ((i<numDevs) && (!gotmatch))
  {
    char devname[1024];
    getAvailableDeviceName(i, devname);

    if (strcmp(devname, name) == 0)
    {  gotmatch = true;
    }
    else
    {  i++;
    }
  }

  if (gotmatch)
  {  return MidiInDevice::fromMidiDeviceNumber(i);
  }
  else
  {  return NULL;
  }
}



MidiInDevice*  MidiInDevice::fromMidiDeviceNumber( int number )
{
  int max = MidiInDevice::getNumAvailableDevices();

  if ((number >= 0) && (number < max))
  {  return new MidiInDevice(number);
  }
  else
  {   return NULL;
  }
}



MidiInDevice::~MidiInDevice()
{
  shutdown();
}


bool  MidiInDevice::hasMessageWaiting()
{
  return _newMsgFlag;
}



bool  MidiInDevice::readMessage(unsigned char **msgPtr, int *size)
{
  if (!_newMsgFlag)
  {  return false;
  }
  else
  { *msgPtr = _msg;
    *size = _msgSize;
    _newMsgFlag = false;
    _msgSize = 0;

    return true;
  }
}



void  MidiInDevice::setMessage(unsigned char *msg, int size)
{
  unsigned char *ptr = _msg;
  int free = MIDI_IN_BUFFER_SIZE;

  // if still haven't checked the old one, then append this message
  if (_newMsgFlag)
  {
    ptr  += _msgSize;
    free -= _msgSize;
  }

  if (size >= free)
  {
    printf("Error Midi-In message buffer overflow!\n");
  }
  else
  { // copy message into the message buffer
    unsigned char* ptr2 = msg;

    for (int i=0;i<size;i++)
    {
      *ptr = *ptr2;
      ptr++;
      ptr2++;
    }

    _msgSize += size;
    _newMsgFlag = true;
  }
}



bool  MidiInDevice::hasDataWaiting()
{
  return _newDataFlag;
}


int  MidiInDevice::readData( unsigned char* &statusBytes,
                             unsigned char* &data1Bytes,
                             unsigned char* &data2Bytes )
{
  if (!_newDataFlag)
  {  return 0;
  }
  else
  {
    int size = _dataSize;
    statusBytes = _dataStatus;
    data1Bytes = _data1;
    data2Bytes = _data2;
    _dataSize = 0;
    _newDataFlag = false;

    return size;
  }
}



void  MidiInDevice::addData( unsigned char status,
                             unsigned char data1,
                             unsigned char data2 )
{
  if (_dataSize >= MIDI_IN_DATA_BUFFER_SIZE)
  {
    printf("Error Midi-In data buffer overflow!\n");
  }
  else
  {
    _dataStatus[_dataSize] = status;
    _data1[_dataSize] = data1;
    _data2[_dataSize] = data2;
    _newDataFlag = true;
    _dataSize++;
  }
}


// ----------------------------------------------------------------------------


MidiOutDevice  *MidiOutDevice::fromMidiDeviceName( const char *name )
{
  int numDevs = MidiOutDevice::getNumAvailableDevices();
  int i=0;
  bool gotmatch = false;

  while ((i<numDevs) && (!gotmatch))
  {
    char devname[1024];
    getAvailableDeviceName(i, devname);

    if (strcmp(devname, name) == 0)
    {  gotmatch = true;
    }
    else
    {  i++;
    }
  }

  if (gotmatch)
  {  return MidiOutDevice::fromMidiDeviceNumber(i);
  }
  else
  {  return NULL;
  }
}



MidiOutDevice  *MidiOutDevice::fromMidiDeviceNumber( int  number )
{
  int max = MidiOutDevice::getNumAvailableDevices();

  if ((number >= 0) && (number < max))
  {  return new MidiOutDevice(number);
  }
  else
  {  return NULL;
  }
}


} // end namespace
