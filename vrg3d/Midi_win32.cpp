//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

#include "Midi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


namespace VRG3D {


  void reportMidiInError(unsigned int error)
  {
    char errorMsg[256];
    midiInGetErrorText(error, errorMsg, 256);
    printf("MidiIn Error %s\n", errorMsg);
    exit(1);
  }


  void reportMidiOutError(unsigned int error)
  {
    char errorMsg[256];
    midiOutGetErrorText(error, errorMsg, 256);
    printf("MidiOut Error %s\n", errorMsg);
    exit(1);
  }


  // Windows uses a callback mechanism, linux needs to be polled
  void CALLBACK  midiInWin32Callback( HMIDIIN  handle,
                                      WORD     midiMessage,
                                      DWORD    dwInstance,
                                      DWORD    data,
                                      DWORD    timestamp )
  {
    MidiInDevice *thisDevice = (MidiInDevice*)dwInstance;

    switch (midiMessage)
    {

    case MIM_OPEN:
    {  break;
    }
    case MIM_CLOSE:
    { printf("MidiIn Callback: Closing Midi.\n");
      break;
    }
    case MIM_ERROR:
    { printf("MidiIn Callback: Midi Error.\n");
      break;
    }
    case MIM_LONGERROR:
    { printf("MidiIn Callback: Error receiving system-exclusive message.\n");

      // You'll get here on the first message from the ICubeX device if
      // it's already running and reporting data.  Do an addBuffer anyway
      // because otherwise, you won't get any more callbacks.
      //
      if (thisDevice->continueCallback)
      {
        LPMIDIHDR lpMIDIHeader = (LPMIDIHDR)data;
        midiInAddBuffer(handle, lpMIDIHeader, sizeof(MIDIHDR));
      }

      break;
    }
    case MIM_MOREDATA:
    { printf("MidiIn Callback: Application is not processing "
             "midi events fast enough.\n");
      break;
    }
    case MIM_DATA:
    { thisDevice->addData( LOBYTE(LOWORD(data)),
                           HIBYTE(LOWORD(data)),
                           LOBYTE(HIWORD(data)));

      if (thisDevice->continueCallback)
      {
        // Queue the buffer for additional input
        LPMIDIHDR lpMIDIHeader = (LPMIDIHDR)data;
        midiInAddBuffer(handle, lpMIDIHeader, sizeof(MIDIHDR));
      }

      break;
    }
    case MIM_LONGDATA:
    { // MIM_LONGDATA indicates a system-exclusive buffer is being sent

      LPMIDIHDR lpMIDIHeader = (LPMIDIHDR)data;
      // a pointer to the bytes received in the midi buffer
      unsigned char *ptr = (unsigned char *)(lpMIDIHeader->lpData);
      unsigned char lastbyte = *(ptr + lpMIDIHeader->dwBytesRecorded - 1);

      // For system-exclusive midi messages:
      //   0xF0 denotes the start of the message
      //   0xF7 denotes the end of the message
      if (lastbyte == 0xF7)
      {
         // End of message reached, process the message
         // The first byte of the message will always be 0xF0, indicating
         // start-of-sysex-message, so skip it and also skip the last
         // byte 0xF7 = end-of-sysex-message

         int size = lpMIDIHeader->dwBytesRecorded - 2;
         thisDevice->setMessage(ptr+1, size);
      }

      if (thisDevice->continueCallback)
      {
        // Queue the buffer for additional input
        midiInAddBuffer(handle, lpMIDIHeader, sizeof(MIDIHDR));
      }

      break;
    }
    default:
    {  printf("MidiIn Callback: Unknown midi message type.\n");
       break;
    }
    }  // end switch
}



MidiInDevice::MidiInDevice( int number )
{
   _msgSize = 0;
   _newMsgFlag = false;
   _dataSize = 0;
   _newDataFlag = false;

   continueCallback = true;

   UINT error = midiInOpen( &_handle, number,
                            (DWORD)midiInWin32Callback,
                            (DWORD)this, CALLBACK_FUNCTION );

    if (error)
    {  reportMidiInError(error);
       return;
    }

    error = midiInReset(_handle);

    if (error)
    {  reportMidiInError(error);
       return;
    }

    _header.lpData = (LPSTR)_sysExBuffer;
    _header.dwBufferLength = sizeof(_sysExBuffer);
    _header.dwFlags = 0;

    error = midiInPrepareHeader(_handle, &_header, sizeof(MIDIHDR));

    if (error)
    { reportMidiInError(error);
      return;
    }

    error = midiInAddBuffer(_handle, &_header, sizeof(MIDIHDR));

    if (error)
    { reportMidiInError(error);
      return;
    }

    error = midiInStart(_handle);

    if (error)
    { reportMidiInError(error);
      return;
    }
}


void  MidiInDevice::shutdown()
{
    //cout << "Closing MidiIn." << endl;
    continueCallback = false;
    midiInStop(_handle);
}


int  MidiInDevice::getNumAvailableDevices()
{
    return  midiInGetNumDevs();
}


void MidiInDevice::getAvailableDeviceName(int number, char *name)
{
    strcpy(name, "");
    int numDevs = getNumAvailableDevices();

    if ((number < 0) || (number >= numDevs))
    {  return;
    }

    MIDIINCAPS mic;
    UINT error = midiInGetDevCaps(number, &mic, sizeof(MIDIINCAPS));

    if (error != 0)
    {  reportMidiInError(error);
    }
    else
    {  strcpy(name, mic.szPname);
    }
}


void  MidiInDevice::poll()  {}


MidiOutDevice::MidiOutDevice(int number)
{
    UINT error = midiOutOpen(&_handle, number, 0, 0, CALLBACK_NULL);

    if (error)
    { reportMidiOutError(error);
      return;
    }

    error = midiOutReset(_handle);

    if (error)
    { reportMidiOutError(error);
      return;
    }
}


MidiOutDevice::~MidiOutDevice()
{
    midiOutClose(_handle);
}


int  MidiOutDevice::getNumAvailableDevices()
{
    return  midiOutGetNumDevs();
}


void MidiOutDevice::getAvailableDeviceName(int number, char *name)
{
   strcpy(name, "");
   int numDevs = getNumAvailableDevices();

   if ((number < 0) || (number >= numDevs))
   {  return;
   }

   MIDIOUTCAPS moc;
   UINT error = midiOutGetDevCaps(number, &moc, sizeof(MIDIOUTCAPS));

   if (error != 0)
   {  reportMidiOutError(error);
   }
   else
   {  strcpy(name, moc.szPname);
   }
}



void  MidiOutDevice::sendMessage(unsigned char* message, int size)
{
  MIDIHDR header;
  header.lpData = (LPSTR)message;
  header.dwBufferLength = size;
  header.dwFlags = 0;

  UINT error = midiOutPrepareHeader(_handle, &header, sizeof(MIDIHDR));

  if (error)
  {  reportMidiOutError(error);
     return;
  }

  // Send the system-exclusive message
  error = midiOutLongMsg(_handle, &header, sizeof(MIDIHDR));

  if (error)
  {  reportMidiOutError(error);
     return;
  }

  // Wait until buffer can be unprepared.
  while (MIDIERR_STILLPLAYING == midiOutUnprepareHeader( _handle,
                                                        &header,
                                                        sizeof(MIDIHDR)))
  {}
}


} // end namespace
