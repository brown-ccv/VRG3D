//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


#include "MouseToTracker.h"
#include <sstream>

namespace VRG3D {

using namespace G3D;

MouseToTracker::MouseToTracker(ProjectionVRCameraRef camera, int numTrackers)
{
  _camera = camera;
  _numTrackers = numTrackers;
  CoordinateFrame initialFrame;
  _rot   = initialFrame.rotation;
  _trans = initialFrame.translation;
  _curTracker = 1;
  _fsaState = START;
}


MouseToTracker::~MouseToTracker()
{
}

std::string
MouseToTracker::intToString(int i)
{
  std::ostringstream ostr;
  // if using strstream rather than stringstream, the following call
  // requires a   << "\0"   at the end.
  ostr << i;
  return std::string(ostr.str());
}


void
MouseToTracker::doUserInput(const Array<EventRef> &events, Array<EventRef> &generatedEvents)
{
  for (int i=0;i<events.size();i++) {
    if (events[i]->getName() == "kbd_TAB_down") {
      cycleTracker();
    }
    else if ((events[i]->getName() == "kbd_X_down") && (_fsaState == START)) {
      _fsaState = XROT;
    }
    else if ((events[i]->getName() == "kbd_Y_down") && (_fsaState == START)) {
      _fsaState = YROT;
    }
    else if ((events[i]->getName() == "kbd_Z_down") && (_fsaState == START)) {
      _fsaState = ZROT;
    }
    else if ((events[i]->getName() == "kbd_SHIFT_down") && (_fsaState == START)) {
      _fsaState = DOLLY;
    }
    else if ((events[i]->getName() == "kbd_X_up") && (_fsaState == XROT)) {
      _fsaState = START;
    }
    else if ((events[i]->getName() == "kbd_Y_up") && (_fsaState == YROT)) {
      _fsaState = START;
    }
    else if ((events[i]->getName() == "kbd_Z_up") && (_fsaState == ZROT)) {
      _fsaState = START;
    }
    else if ((events[i]->getName() == "kbd_SHIFT_up") && (_fsaState == DOLLY)) {
      _fsaState = START;
    }
    else if (events[i]->getName() == "Mouse_Pointer") {
      if (_fsaState == START) {
        generatedEvents.append(mouseMoveInPlane(events[i]));
      }
      else if (_fsaState == XROT) {
        generatedEvents.append(mouseMoveRotX(events[i]));
      }
      else if (_fsaState == YROT) {
        generatedEvents.append(mouseMoveRotY(events[i]));
      }
      else if (_fsaState == ZROT) {
        generatedEvents.append(mouseMoveRotZ(events[i]));
      }
      else if (_fsaState == DOLLY) {
        generatedEvents.append(mouseMoveInOut(events[i]));
      }
    }
  }
}


void
MouseToTracker::cycleTracker()
{
  if (_numTrackers == 1) {
    return;
  }
  else {
    _curTracker++;
    if (_curTracker > _numTrackers) {
      _curTracker = 1;
    }
  }
}

EventRef
MouseToTracker::mouseMoveInPlane(EventRef e)
{
  // Mouse coordinates are reported from -1 -> 1, normalize to 0 -> 1
  Vector2 v = e->get2DData();
  double x = (v[0] / 2.0) + 0.5;
  double y = (v[1] / 2.0) + 0.5;
  //cout << "v = " << v << endl;
  //cout << "x=" << x << endl;
  //cout << "y=" << y << endl;

  Vector3 xvec = _camera->tile.topRight - _camera->tile.topLeft;
  Vector3 yvec = _camera->tile.topRight - _camera->tile.botRight;
  //cout << "xvec=" << xvec << endl;
  //cout << "yvec=" << yvec << endl;

  // Point on the filmplane
  Vector3 fpPoint = _camera->tile.botLeft + (x * xvec) + (y * yvec);
  //cout << "p=" << p << endl;

  Vector3 cameraPos = _camera->getCameraPos();
  Vector3 dir = fpPoint - cameraPos;
  //cout << "fp=" << fpPoint << endl;
  //cout << "dir=" << dir << endl;

  double z = _trans[2];

  Vector3 xy_normal(0,0,-1);
  Plane xyPlane(xy_normal, Vector3(0,0,z));

  //Ray r = Ray::fromOriginAndDirection(cameraPos, dir);
  Ray r = Ray::fromOriginAndDirection(cameraPos, dir.unit());  //+++

  Vector3 intersect = r.intersection(xyPlane);
  if (intersect == Vector3::inf()) {
    // try flipping the normal on the xyPlane
    Plane xyPlane2(-xy_normal, Vector3(0,0,z));
    intersect = r.intersection(xyPlane2);
  }

  _trans[0] = intersect[0];
  _trans[1] = intersect[1];
  _lastPos = e->get2DData();
  return (new Event("Mouse" + intToString(_curTracker) + "_Tracker", CoordinateFrame(_rot,_trans)));
}

EventRef
MouseToTracker::mouseMoveInOut(EventRef e)
{
  double dollyFactor = -4.0;
  Vector3 d(0, 0, dollyFactor * (e->get2DData()[1] - _lastPos[1]));
  _trans = _trans + d;
  _lastPos = e->get2DData();
  return (new Event("Mouse" + intToString(_curTracker) + "_Tracker", CoordinateFrame(_rot,_trans)));
}

EventRef
MouseToTracker::mouseMoveRotX(EventRef e)
{
  double rotFactor = 2.0;
  double dx = e->get2DData()[0] - _lastPos[0];
  double angle = (dx/2.0) * rotFactor;
  _rot = Matrix3::fromAxisAngle(Vector3(1,0,0), angle) * _rot;
  _lastPos = e->get2DData();
  return (new Event("Mouse" + intToString(_curTracker) + "_Tracker", CoordinateFrame(_rot,_trans)));
}

EventRef
MouseToTracker::mouseMoveRotY(EventRef e)
{
  double rotFactor = 2.0;
  double dx = e->get2DData()[0] - _lastPos[0];
  double angle = (dx/2.0) * rotFactor;
  _rot = Matrix3::fromAxisAngle(Vector3(0,1,0), angle) * _rot;
  _lastPos = e->get2DData();
  return (new Event("Mouse" + intToString(_curTracker) + "_Tracker", CoordinateFrame(_rot,_trans)));
}

EventRef
MouseToTracker::mouseMoveRotZ(EventRef e)
{
  double rotFactor = 2.0;
  double dx = e->get2DData()[0] - _lastPos[0];
  double angle = (dx/2.0) * rotFactor;
  _rot = Matrix3::fromAxisAngle(Vector3(0,0,1), angle) * _rot;
  _lastPos = e->get2DData();
  return (new Event("Mouse" + intToString(_curTracker) + "_Tracker", CoordinateFrame(_rot,_trans)));
}

} // end namespace


