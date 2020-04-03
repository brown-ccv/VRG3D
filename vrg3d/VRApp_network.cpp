//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


#include  "VRApp.h"
#include  "EventNet.h"

//#include <iostream>

using namespace std;


namespace VRG3D {

using namespace G3D;

void  VRApp::clientSetup()
{
  // instance() call creates a new NetworkDevice and calls init() on it
  _networkDevice = NetworkDevice::instance();
  NetAddress serverAddress(_clusterServerName, CLUSTER_NET_PORT);
  cout << "Client: Trying to connect to " << serverAddress.toString() << endl;
  // Looks like it should set TCP_NODELAY
  _conduitToServer = ReliableConduit::create(serverAddress);

  // If didn't connect to server right away, keep trying for a maximum of 10 sec.
  double timeout = System::time() + 11.0;
  while (((_conduitToServer.isNull()) || (!_conduitToServer->ok())) &&
         (System::time() < timeout))
  {
    vrg3dSleepSecs(1.0);
    _conduitToServer = NULL;
    cout << "Client: trying again.." << endl;
    _conduitToServer = ReliableConduit::create(serverAddress);
  }

  if (_conduitToServer.isNull() || !_conduitToServer->ok())
  {
    cout << "Client: Unable to connect to server." << endl;
    _endProgram = true;
  }
  else
  { cout << "Client: Connected ok." << endl;
  }
}


void  VRApp::clientRequestEvents()
{
  clientCheckConnection();
  EventNetMsg requestMsg(new Event("ReqEvents"));
  _conduitToServer->send(requestMsg.type(), requestMsg);
}


void  VRApp::clientCheckConnection()
{
  if (!_conduitToServer->ok()) {
    cerr << "Connection to server lost." << endl;
    exit(0);
  }
}


void  VRApp::clientReceiveEvents(Array<EventRef> &events)
{
  while (!_conduitToServer->messageWaiting())
  {
    clientCheckConnection();
  }

  if (_conduitToServer->waitingMessageType() != EVENTBUFFERNETMSG_TYPE)
  {
    cerr << "Client: Expected event buffer msg, got something else." << endl;
    _endProgram = true;
  }
  else
  { EventBufferNetMsg msg;
    clientCheckConnection();
    _conduitToServer->receive(msg);
    events.append(msg.eventBuffer);
  }
}


void  VRApp::clientRequestAndWaitForOkToSwap()
{
  clientCheckConnection();

  // send a request to swap
  EventNetMsg requestMsg(new Event("Swap"));
  _conduitToServer->send(requestMsg.type(), requestMsg);

  // wait for reply
  while (!_conduitToServer->messageWaiting())
  {
    clientCheckConnection();
  }

  if (_conduitToServer->waitingMessageType() != EVENTNETMSG_TYPE) {
    cerr << "Client: Expected SwapOK msg, got something else." << endl;
    _endProgram = true;
  }
  else
  { clientCheckConnection();
    EventNetMsg replyMsg;
    _conduitToServer->receive(replyMsg);
    debugAssert(replyMsg.event->getName() == "SwapOK");
  }
}


} // end namespace
