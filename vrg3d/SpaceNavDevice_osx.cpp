//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

// OSX Specific Implementation

using namespace G3D;

// Include the 3D Connexion API from /Library/Frameworks
#include <3DconnexionClient/ConnexionClientAPI.h>

UInt16 gClientID;
GMutex  gEventBufferMutex;
static Array<VRG3D::EventRef> gEventBuffer;
G3D::Stopwatch gClockPump;
G3D::Stopwatch gClockTwistRight;
G3D::Stopwatch gClockTwistLeft;

// Mark these function for weak-linking to avoid runtime issues if the
// 3dconnexion framework is not installed -- not sure if we really
// need this, since we are statically linking.
//
extern OSErr InstallConnexionHandlers(ConnexionMessageHandlerProc messageHandler,
                                      ConnexionAddedHandlerProc addedHandler,
                                      ConnexionRemovedHandlerProc removedHandler)__attribute__((weak_import));

namespace VRG3D {


class TdxDeviceEvents
{
public:
  TdxDeviceEvents();
  virtual ~TdxDeviceEvents();

  static OSStatus threadProc(void *param);

private:
  EventQueueRef m_mainEventQueue;
};


void messageHandlerProc(io_connect_t connection, natural_t messageType,
                        void *messageArgument)
{


  ConnexionDeviceStatePtr state;
  state = (ConnexionDeviceStatePtr)messageArgument;
  static bool sPushedDown = false;
  static bool sTwistRight = false;
  static bool sTwistLeft = false;
  static bool sTimedTwistRight = false;
  static bool sTimedTwistLeft = false;

  switch(messageType)
    {
    case kConnexionMsgDeviceState:
      /* Device state messages are broadcast to all clients.  It is up to
       * the client to figure out if the message is meant for them. This
       * is done by comparing the "client" id sent in the message to our
       * assigned id when the connection to the driver was established.
       */
      if (state->client == gClientID)
    {
      /* report = the kind of device data that's being reported */
      switch (state->command)
        {
        case kConnexionCmdHandleAxis:
          {
    static SInt16 zerodata[] = {0, 0, 0, 0, 0, 0};
    static Boolean isZero = FALSE;
    static Boolean wasZero = FALSE;

    if (isZero = (memcmp(state->axis, zerodata, sizeof(zerodata)) == 0)) {
      if (wasZero == FALSE) {
        Vector3 trans = Vector3::zero();
        Vector3 rot = Vector3::zero();
        //cout << "Trans: " << trans << endl;
        //cout << "Rot:   " << rot << endl;

        if (trans[2] > 185  && !sPushedDown) {
          sPushedDown = true;
          gClockPump.tick();
        }

        // Check for "pump" gesture.
        if (trans[2] < 40 && sPushedDown) {
          gClockPump.tock();
          sPushedDown = false;

          if (gClockPump.elapsedTime() < 0.35f) {
    gEventBufferMutex.lock();
    gEventBuffer.append(new VRG3D::Event("SpaceNav_Pump_Gesture"));
    gEventBufferMutex.unlock();
          }
        }

        if (rot[2] > 130 && !sTwistRight) {
          sTwistRight = true;
          //cout << "Twisted right!" << endl;
        }

        if (sTwistRight && rot[2] < 75) {
          // A single right twist has occurred.
          // Queue event.
          gEventBufferMutex.lock();
          gEventBuffer.append(new VRG3D::Event("SpaceNav_TwistRight"));
          gEventBufferMutex.unlock();

          // Start timer to check for double twist gesture.
          gClockTwistRight.tick();
          sTimedTwistRight = true;
          sTwistRight = false;
        }

        if (sTimedTwistRight && sTwistRight) {
          gClockTwistRight.tock();

          if (gClockTwistRight.elapsedTime() < 0.3f) {
    // Queue event.
    gEventBufferMutex.lock();
    gEventBuffer.append(new VRG3D::Event("SpaceNav_DoubleTwistRight"));
    gEventBufferMutex.unlock();
    // cout << "Double twist right!" << endl;
    sTimedTwistRight = false;
    sTwistRight = false;
          } else {
    sTimedTwistRight = false;
          }
        }

        if (rot[2] < -150 && !sTwistLeft) {
          sTwistLeft = true;
          //cout << "Twisted left!" << endl;
        }

        if (sTwistLeft && rot[2] > -75) {
          // A single left twist has occurred.
          // Queue event.
          gEventBufferMutex.lock();
          gEventBuffer.append(new VRG3D::Event("SpaceNav_TwistLeft"));
          gEventBufferMutex.unlock();

          // Start timer to check for double twist gesture.
          gClockTwistLeft.tick();
          sTimedTwistLeft = true;
          sTwistLeft = false;
        }

        if (sTimedTwistLeft && sTwistLeft) {
          gClockTwistLeft.tock();

          if (gClockTwistLeft.elapsedTime() < 0.3f) {
    // Queue event.
    gEventBufferMutex.lock();
    gEventBuffer.append(new VRG3D::Event("SpaceNav_DoubleTwistLeft"));
    gEventBufferMutex.unlock();
    //cout << "Double twist left!" << endl;
    sTimedTwistLeft = false;
    sTwistLeft = false;
          } else {
    sTimedTwistLeft = false;
          }
        }

        gEventBufferMutex.lock();
        gEventBuffer.append(new VRG3D::Event("SpaceNav_Trans", trans));
        gEventBuffer.append(new VRG3D::Event("SpaceNav_Rot", rot));
        gEventBufferMutex.unlock();
      }
    }
    else {
      Vector3 trans(state->axis[0], state->axis[1], state->axis[2]);
      Vector3 rot(state->axis[3], state->axis[4], state->axis[5]);
      //cout << "Trans: " << trans << endl;
      //cout << "Rot:   " << rot << endl;

      if (trans[2] > 185 && !sPushedDown) {
        sPushedDown = true;
        gClockPump.tick();
      }

      // Check for "pump" gesture.
      if (trans[2] < 40 && sPushedDown) {
         gClockPump.tock();
         sPushedDown = false;
         if (gClockPump.elapsedTime() < 0.35f) {
           gEventBufferMutex.lock();
           gEventBuffer.append(new VRG3D::Event("SpaceNav_Pump_Gesture"));
           gEventBufferMutex.unlock();
         }
      }

      if (rot[2] > 130 && !sTwistRight) {
        sTwistRight = true;
        //cout << "Twisted right!" << endl;
      }

      if (sTwistRight && rot[2] < 75) {
        // A single right twist has occurred.
        // Queue event.
        gEventBufferMutex.lock();
        gEventBuffer.append(new VRG3D::Event("SpaceNav_TwistRight"));
        gEventBufferMutex.unlock();

        // Start timer to check for double twist gesture.
        gClockTwistRight.tick();
        sTimedTwistRight = true;
        sTwistRight = false;
      }

      if (sTimedTwistRight && sTwistRight) {
        gClockTwistRight.tock();

        if (gClockTwistRight.elapsedTime() < 0.3f) {
          // Queue event.
          gEventBufferMutex.lock();
          gEventBuffer.append(new VRG3D::Event("SpaceNav_DoubleTwistRight"));
          gEventBufferMutex.unlock();
          //cout << "Double twist right!" << endl;
          sTimedTwistRight = false;
          sTwistRight = false;
        } else {
          sTimedTwistRight = false;
        }
      }

      if (rot[2] < -130 && !sTwistLeft) {
        sTwistLeft = true;
        //cout << "Twisted left!" << endl;
      }

      if (sTwistLeft && rot[2] > -75) {
        // A single left twist has occurred.
        // Queue event.
        gEventBufferMutex.lock();
        gEventBuffer.append(new VRG3D::Event("SpaceNav_TwistLeft"));
        gEventBufferMutex.unlock();

        // Start timer to check for a double twist.
        gClockTwistLeft.tick();
        sTimedTwistLeft = true;
        sTwistLeft = false;
      }

      if (sTimedTwistLeft && sTwistLeft) {
        gClockTwistLeft.tock();

        if (gClockTwistLeft.elapsedTime() < 0.3f) {
          // Queue event.
          gEventBufferMutex.lock();
          gEventBuffer.append(new VRG3D::Event("SpaceNav_DoubleTwistLeft"));
          gEventBufferMutex.unlock();
          //cout << "Double twist left!" << endl;
          sTimedTwistLeft = false;
          sTwistLeft = false;
        } else {
          sTimedTwistLeft = false;
        }
      }

      gEventBufferMutex.lock();
      gEventBuffer.append(new VRG3D::Event("SpaceNav_Trans", trans));
      gEventBuffer.append(new VRG3D::Event("SpaceNav_Rot", rot));
      gEventBufferMutex.unlock();
    }
    break;
          }

        case kConnexionCmdAppSpecific:
          break;

        case kConnexionCmdHandleButtons:
          {
    static bool buttonPressed[] = {false, false};
    if ((state->buttons & 1) && (!buttonPressed[0])) {
      //cout << "btn 1 down" << endl;
      gEventBufferMutex.lock();
      gEventBuffer.append(new VRG3D::Event("SpaceNav_Btn1_down"));
      gEventBufferMutex.unlock();
      buttonPressed[0] = true;
    }
    else if ((!(state->buttons & 1)) && (buttonPressed[0])) {
      //cout << "btn 1 up" << endl;
      gEventBufferMutex.lock();
      gEventBuffer.append(new VRG3D::Event("SpaceNav_Btn1_up"));
      gEventBufferMutex.unlock();
      buttonPressed[0] = false;
    }
    if ((state->buttons & 2) && (!buttonPressed[1])) {
      //cout << "btn 2 down" << endl;
      gEventBufferMutex.lock();
      gEventBuffer.append(new VRG3D::Event("SpaceNav_Btn2_down"));
      gEventBufferMutex.unlock();
      buttonPressed[1] = true;
    }
    else if ((!(state->buttons & 2)) && (buttonPressed[1])) {
      //cout << "btn 2 up" << endl;
      gEventBufferMutex.lock();
      gEventBuffer.append(new VRG3D::Event("SpaceNav_Btn2_up"));
      gEventBufferMutex.unlock();
      buttonPressed[1] = false;
    }
    break;
          }
        }
    }
      break;

    default:
      // other messageTypes can happen and should be ignored
      break;
    }
}


TdxDeviceEvents::TdxDeviceEvents()
{
  // Check if the driver is installed
  if (InstallConnexionHandlers == NULL)
    return;

  /* must save the main event queue while we're running within the main thread.
   * GetMainEventQueue() is not thread-safe and will cause big problems if
   * called from the spawned thread
   */
  m_mainEventQueue = GetMainEventQueue();

  // Create the listening thread for device events
  MPCreateTask(&threadProc, 0, 512000, NULL, NULL, NULL, 0, NULL);
}


TdxDeviceEvents::~TdxDeviceEvents()
{
  // Check if the driver is installed
  if (InstallConnexionHandlers == NULL)
    return;

  if (gClientID)
    UnregisterConnexionClient(gClientID);
  CleanupConnexionHandlers();
}


OSStatus
TdxDeviceEvents::threadProc(void* param)
{
  if (InstallConnexionHandlers(messageHandlerProc, 0L, 0L) == noErr)  {
    gClientID = RegisterConnexionClient(kConnexionClientWildcard,
                                        NULL,
                                        kConnexionClientModeTakeOver,
                                        kConnexionMaskAll);
    SInt32 result = 0;
    //cout << "Calibrating space nav" << endl;
    ConnexionControl(kConnexionCtlCalibrate, 0, &result);
    //cout << "Calibrated" << endl;
    RunCurrentEventLoop(kEventDurationForever);
  }
  else
  { cerr << "Error initializing space navigator device." << endl;
  }

  return noErr;
}


void
SpaceNavDevice::setup()
{
  new TdxDeviceEvents();
}


void  SpaceNavDevice::pollForInput(Array<EventRef> &events)
{
  gEventBufferMutex.lock();
  events.append(gEventBuffer);
  gEventBuffer.clear();
  gEventBufferMutex.unlock();
}


} // end namespace
