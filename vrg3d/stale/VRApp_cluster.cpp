//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


// This is a stale version using synchit rather than g3d networking.
// It is only here as an archive and is not supported.


#include  "VRApp.h"
#include  "EventNet.h"

//#include <iostream>

using namespace std;


namespace VRG3D {


void  VRApp::clientSetup()
{
  _clusterSynchIt = new SynchItClient();
  //_clusterSynchIt->Init(VRG3D_DEFAULT_SYNCHIT_PORT, clusterServerName.c_str());
  _clusterSynchIt->Init(VRG3D_DEFAULT_SYNCHIT_PORT, (char *)_clusterServerName.c_str());
  _clusterSynchItBuf = new char[_clusterSynchIt->max_buffer_size()];
}


void  VRApp::clientRequestEvents()
{
  // no request for cluster-sync
}


void  VRApp::clientCheckConnection()
{}


void  VRApp::clientReceiveEvents(Array<EventRef> &events)
{
  int num = _clusterSynchIt->GetNewData(_clusterSynchItBuf);
  if (num > 0)
  {
    // Convert to BinaryInput to easily deserialize the data
    BinaryInput bin( (const uint8*)_clusterSynchItBuf,
                     num, G3D_LITTLE_ENDIAN, false, false);

    EventBufferNetMsg msg;
    msg.deserialize(bin);
    events.append(msg.eventBuffer);
  }
}


void  VRApp::clientRequestAndWaitForOkToSwap()
{
  // currently no software swap lock
}

} // end namespace
