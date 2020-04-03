/**
 * \author Daniel Keefe (dfk)
 *
 * \file  ViewerHCI.H
 * \brief
 *
 */


#ifndef VIEWERHCI_H
#define VIEWERHCI_H

#include "CommonInc.H"
#include "EventMgr.H"
#include "Fsa.H"
#include "GfxMgr.H"

namespace VRBase {

/**
  This viewer assumes the model is centered around (0,0,0).

  Controls:

  Mouse with left btn - trackball rotate
  Arrow keys          - pan left, right, up, and down
  "I" and "O"         - dolly in and out
  PAGEUP and PAGEDOWN - scale model up/down
  SPACEBAR            - start/stop rocking the model back and forth
  "F" and "S"         - make rocking faster or slower
  "W" and "N"         - make rocking angle wider or narrower
*/
class ViewerHCI
{
public:  
  ViewerHCI(EventMgrRef eventMgr, GfxMgrRef gfxMgr);
  virtual ~ViewerHCI();

  void trackballOn(VRG3D::EventRef e);
  void trackballMove(VRG3D::EventRef e);
  void trackballOff(VRG3D::EventRef e);

  void toggleRocking(VRG3D::EventRef e);
  void speedUpRocking(VRG3D::EventRef e);
  void slowDownRocking(VRG3D::EventRef e);
  void incRockingAngle(VRG3D::EventRef e);
  void decRockingAngle(VRG3D::EventRef e);

  void panLeft(VRG3D::EventRef e);
  void panRight(VRG3D::EventRef e);
  void panUp(VRG3D::EventRef e);
  void panDown(VRG3D::EventRef e);
  void dollyIn(VRG3D::EventRef e);
  void dollyOut(VRG3D::EventRef e);
  void scaleUp(VRG3D::EventRef e);
  void scaleDown(VRG3D::EventRef e);

  void poseForAnimation(G3D::Array<G3D::SurfaceRef> &posedModels, const G3D::CoordinateFrame &virtualToRoomSpace);

  void setRotCenter(G3D::Vector3 v){
    _boundingSphere.center=v;
  }

protected:
  EventMgrRef     _eventMgr;
  FsaRef          _fsa;
  GfxMgrRef       _gfxMgr;
  double          _lastRotTime;
  double          _rotAngularVel;
  G3D::Sphere          _boundingSphere;
  G3D::Vector3         _lastIntersectionPt;
  int             _animationCallbackID;
  double          _panAmt;
  bool            _rocking;
  double          _rockingDir;
  double          _rockingAngle;
  double          _totalAngle;
};

}
#endif
