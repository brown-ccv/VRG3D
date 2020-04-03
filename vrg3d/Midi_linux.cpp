//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

#include "Midi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>


namespace VRG3D {

void reportMidiInError(char *errorMsg)
{
    printf("MidiIn Error %s\n", errorMsg);
    exit(1);
}


void reportMidiOutError(char *errorMsg)
{
    printf("MidiOut Error %s\n", errorMsg);
    exit(1);
}


MidiInDevice::MidiInDevice(int number)
{
   _msgSize = 0;
   _newMsgFlag = false;
   _dataSize = 0;
   _newDataFlag = false;

   _sysExMsgSize = 0;
   char devName[1024];
   MidiInDevice::getAvailableDeviceName(number, devName);

   //cout << "about to open MidiIn" << endl;
   _handle = open(devName, O_RDONLY, 0);

   if (_handle < 0)
   {  printf("MidiIn cannot open %s\n", devName);
      exit(1);

      return;
   }
}


void  MidiInDevice::shutdown()
{
  //cout << "Closing MidiIn." << endl;
}


int  MidiInDevice::getNumAvailableDevices()
{
    return 4;
}


void  MidiInDevice::getAvailableDeviceName(int number, char *name)
{
    strcpy(name, "");
    int  numDevs = getNumAvailableDevices();

    if ((number < 0) || (number >= numDevs))
    {  return;
    }

    if (number == 0)
    {  strcpy(name, "/dev/midi00");
    }
    else if (number == 1)
    {  strcpy(name, "/dev/midi01");
    }
    else if (number == 2)
    {  strcpy(name, "/dev/midi02");
    }
    else if (number == 3)
    {  strcpy(name, "/dev/midi03");
    }
    else
    {  strcpy(name, "");
    }
}


void MidiInDevice::poll()
{
  if (_handle >= 0)
  {
    unsigned char *newMsgStart = _sysExBuffer + _sysExMsgSize;

    int count = read( _handle, (void*)newMsgStart,
                      MIDI_IN_BUFFER_SIZE - _sysExMsgSize );

    _sysExMsgSize += count;

    if (_sysExMsgSize >= MIDI_IN_BUFFER_SIZE)
    {
      printf("Error: MidiIn Buffer Overflow.\n");
      _sysExMsgSize = 0;
    }

    if (count > 0)
    {
      unsigned char lastbyte = _sysExBuffer[_sysExMsgSize-1];

      //cout << "first byte = " << (int)(_sysExBuffer[0]) << endl;
      //cout << "last byte = " << (int)lastbyte << endl;

      //for (int i=0;i<count;i++) {
      //  printf("%d: %X\n", i, (int)newMsgStart[i]);
      //}

      if (lastbyte == 0xF7)
      {
        // got end of message signal, skip first byte and last and set
        // the rest as the data message
        setMessage(_sysExBuffer+1, _sysExMsgSize-2);
        _sysExMsgSize = 0;
      }
    }
  }
}


// ----------------------------------------------------------------------------


MidiOutDevice::MidiOutDevice(int number)
{
    char devName[1024];
    MidiOutDevice::getAvailableDeviceName(number, devName);

    //cout << "about to open MidiOut" << endl;
    _handle = open(devName, O_WRONLY, 0);

    if (_handle < 0)
    {  printf("MidiOut cannot open %s\n", devName);
       exit(1);

       return;
    }
}


MidiOutDevice::~MidiOutDevice()  {}


int  MidiOutDevice::getNumAvailableDevices()
{
    return 4;
}

void  MidiOutDevice::getAvailableDeviceName(int number, char *name)
{
  strcpy(name, "");
  int numDevs = getNumAvailableDevices();

  if ((number < 0) || (number >= numDevs))
  {  // skip
  }
  else if (number == 0)
  {  strcpy(name, "/dev/midi00");
  }
  else if (number == 1)
  {  strcpy(name, "/dev/midi01");
  }
  else if (number == 2)
  {  strcpy(name, "/dev/midi02");
  }
  else if (number == 3)
  {  strcpy(name, "/dev/midi03");
  }
  else
  {  strcpy(name, "");
  }
}


void MidiOutDevice::sendMessage(unsigned char* message, int size)
{
  write(_handle, message, size);
}


} // end namespace
