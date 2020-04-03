//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

#include  <iostream>
#include  <stdio.h>
#include  <stdlib.h>
#include  <signal.h>
#include  <spnav.h> 

#include  "SpaceNavDevice.h"

using namespace std;
using namespace G3D;

namespace VRG3D {

void
SpaceNavDevice::setup()
{
  connected=true;
  if(spnav_open()==-1) {
    fprintf(stderr, "failed to connect to the space navigator daemon\n");
    fprintf(stderr, "Continuing without SpaceNav Support(note: free spacenavd, not propietary driver, must be used.)\n");
    fprintf(stderr, "driver available at: spacenav.sourceforge.net\n");
    connected=false;
  }
}

SpaceNavDevice::~SpaceNavDevice() {
  spnav_close();
}

void  SpaceNavDevice::pollForInput(Array<EventRef> &events) {
  //Just sayin, the open source library is way nicer than the closed
  if(connected){
    spnav_event sev;
    while(spnav_poll_event(&sev)) {//non-blocking API
      if(sev.type == SPNAV_EVENT_MOTION) {
        Vector3 trans(-sev.motion.x, sev.motion.z, sev.motion.y);
        Vector3 rot(-sev.motion.rx, sev.motion.rz, sev.motion.ry);
        trans=trans/-400.0;//roughly normalizes
        rot=rot/-400.0;//roughly normalizes
        events.append(new VRG3D::Event("SpaceNav_Trans", trans));
        events.append(new VRG3D::Event("SpaceNav_Rot", rot));
      } else {	/* SPNAV_EVENT_BUTTON */
        if(sev.button.press) {
        if(sev.button.bnum==0) 
          events.append(new VRG3D::Event("SpaceNav_Btn1_down"));
        else
          events.append(new VRG3D::Event("SpaceNav_Btn2_down"));
        }else{
        if(sev.button.bnum==0) 
          events.append(new VRG3D::Event("SpaceNav_Btn1_up"));
        else
          events.append(new VRG3D::Event("SpaceNav_Btn2_up"));
        }
      }
    }
  }
}


} // end namespace
