/**
 * \author Daniel Keefe (dfk)
 *
 * \file  DefaultMouseNav.H
 * \brief
 *
 */


#ifndef DEFAULTMOUSENAV_H
#define DEFAULTMOUSENAV_H

#include "CommonInc.H"
#include "EventMgr.H"
#include "Fsa.H"
#include "GfxMgr.H"

namespace VRBase {

/**
   This mouse navigation is derived from "Unicam" by Zeleznik et al.

   Relevant ConfigVal's:
   
   "DefaultMouseNav_btn" specifies the button event used, set this to
   something different to use an alternative to the left mouse button.

   "DefaultMouseNav_ptr" specifies the 2D event corresponding to mouse movement.

   "DefaultMouseNav_ptrOffset" is an offset in IS3D screen coords, if
   you have a 3D icon attached to the mouse, then the Z calculation
   for the mouse click will hit that 3D icon rather than the
   underlying geometry that you usually want it to.  This offsets the
   mouse click location in screen coords so you won't hit the
   geometry.
   
   Interaction Description:
   
   - Start state: wait for mouse down then transition to PanDollyRotDecision
     - on mouse down, store an intersection point if the click intersected some geometry

   - PanDollyRotDecision state: 
     - if change in mouseX > threshold goto Pan
     - if change in mouseY > threshold goto Dolly
     - if mouse up, then go to RotWaitForSecondClick
     - if wait for mouse up time limit is reached then goto PanDollyDecision

   - PanDollyDecision state: 
     - if change in mouseX > threshold goto Pan
     - if change in mouseY > threshold goto Dolly

   - Pan state:
     - if geometry was clicked then act like we grabbed the geometry at that point
     and are dragging it in a plane parallel to the filmplane
     - otherwise, grab the point under the mouse on the filmplane

   - Dolly state:
     - vertical mouse movement only controls dollying.
     - if geometry was clicked then dollying is scaled such that moving the mouse to
     the bottom of the screen would pull that geometry right up to the filmplane no matter
     how far away it was when it started.
     - otherwise, a constant dolly factor is used to turn vertical mouse movement into
     translation in and out of the screen.
     
   - Rot state:
     - if we clicked on some geometry do a trackball rotate around the selected 3D point
     - otherwise rotate around the origin of RoomSpace
     - when we exit the state, check our angular velocity - if the mouse is still rotating
     the model around (angular vel > threshold) then goto Spinning

   - Spinning state:
     - continue to rotate the model with the same angular velocity and direction until
     the mouse is clicked once to stop it.
*/
class DefaultMouseNav
{
public:  
  DefaultMouseNav(EventMgrRef eventMgr, GfxMgrRef gfxMgr);
  virtual ~DefaultMouseNav();

  void startEnter(VRG3D::EventRef e);
  void initialClick(VRG3D::EventRef e);
  void checkMovementForPanOrDolly(VRG3D::EventRef e);
  void panMove(VRG3D::EventRef e);
  void dollyMove(VRG3D::EventRef e);
  void rotOn(VRG3D::EventRef e);
  void rotMove(VRG3D::EventRef e);
  void spinDecision(VRG3D::EventRef e);
  void spinningEnter(VRG3D::EventRef e);
  void spinOff(VRG3D::EventRef e);
  
  void scaleUp(VRG3D::EventRef e);
  void scaleDown(VRG3D::EventRef e);

  void draw(G3D::RenderDevice *rd, const G3D::CoordinateFrame &virtualToRoomSpace);
  void poseForAnimation(G3D::Array<G3D::PosedModel::Ref> &posedModels, const G3D::CoordinateFrame &virtualToRoomSpace);

protected:
  EventMgrRef     _eventMgr;
  FsaRef          _fsa;
  GfxMgrRef       _gfxMgr;
  G3D::Vector2    _ptrOffset;
  G3D::Vector2    _lastPos;
  G3D::Vector2    _initialClickPos;
  double          _initialClickTime;
  bool            _hitGeometry;
  G3D::Vector3    _hitPoint;
  G3D::Vector3    _lastIntersectionPt;
  G3D::Sphere     _boundingSphere;
  double          _lastRotTime;
  double          _rotAngularVel;
  G3D::Vector3    _rotAxis;
  bool            _showIcon;
  int             _animationCallbackID;
};

}
#endif
