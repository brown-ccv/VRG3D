#include "DefaultMouseNav.H"

#include "ConfigVal.H"
#include "StringUtils.H"


namespace VRBase {

using namespace G3D;

DefaultMouseNav::DefaultMouseNav(EventMgrRef eventMgr, GfxMgrRef gfxMgr)
{
  _eventMgr = eventMgr;
  _gfxMgr = gfxMgr;
  _showIcon = false;

  std::string btn   = ConfigVal("DefaultMouseNav_btn", "Mouse_Left_Btn", false);
  std::string ptr   = ConfigVal("DefaultMouseNav_ptr", "Mouse_Pointer", false);
  // offset in IS3D screen coords, if you have a 3D icon attached to
  // the mouse, then the Z calculation for the mouse click will hit
  // that 3D icon rather than the underlying geometry that you usually
  // want it to.  This offsets the mouse click location in screen
  // coords so you won't hit the geometry.
  _ptrOffset = ConfigVal("DefaultMouseNav_ptrOffset", Vector2(0,0), false);

  Array<std::string> onTriggers;
  onTriggers.append(btn + "_down");
  Array<std::string> offTriggers;
  offTriggers.append(btn + "_up");
  Array<std::string> moveTriggers;
  moveTriggers.append(ptr);

  _fsa = new Fsa("DefaultMouseNav FSA");

  _fsa->addState("Start");
  _fsa->addState("PanDollyRotDecision");
  _fsa->addState("PanDollyDecision");  
  _fsa->addState("Pan");
  _fsa->addState("Dolly");
  _fsa->addState("RotWaitForSecondClick");
  _fsa->addState("Rot");
  _fsa->addState("Spinning");
  
  // arcs from Start
  _fsa->addArc("InitialClick", "Start", "PanDollyRotDecision", onTriggers);
  _fsa->addArcCallback("InitialClick", this, &DefaultMouseNav::initialClick);
  _fsa->addStateEnterCallback("Start", this, &DefaultMouseNav::startEnter);
  
  // arcs from PanDollyRotDecision
  _fsa->addArc("GotQuickClick", "PanDollyRotDecision", "RotWaitForSecondClick", offTriggers);
  _fsa->addArc("PanDollyRotDecision_Move", "PanDollyRotDecision", "PanDollyRotDecision", moveTriggers);
  _fsa->addArcCallback("PanDollyRotDecision_Move", this, &DefaultMouseNav::checkMovementForPanOrDolly);
  _fsa->addArc("SingleClickTimeout", "PanDollyRotDecision", "PanDollyDecision",
               splitStringIntoArray("DefaultMouseNav_SingleClickTimeout"));

  // arcs from PanDollyDecision
  _fsa->addArc("PanDollyDecision_Move", "PanDollyDecision", "PanDollyDecision", moveTriggers);
  _fsa->addArcCallback("PanDollyDecision_Move", this, &DefaultMouseNav::checkMovementForPanOrDolly);

  // arcs from Pan
  _fsa->addArc("Pan_Move", "Pan", "Pan", moveTriggers);
  _fsa->addArcCallback("Pan_Move", this, &DefaultMouseNav::panMove);
  _fsa->addArc("Pan_Off", "Pan", "Start", offTriggers);

  // arcs from Dolly
  _fsa->addArc("Dolly_Move", "Dolly", "Dolly", moveTriggers);
  _fsa->addArcCallback("Dolly_Move", this, &DefaultMouseNav::dollyMove);
  _fsa->addArc("Dolly_Off", "Dolly", "Start", offTriggers);

  // arcs from RotWaitForSecondClick
  _fsa->addArc("Rot_On", "RotWaitForSecondClick", "Rot", onTriggers);
  _fsa->addArcCallback("Rot_On", this, &DefaultMouseNav::rotOn);

  // arcs from Rot
  _fsa->addArc("Rot_Move", "Rot", "Rot", moveTriggers);
  _fsa->addArcCallback("Rot_Move", this, &DefaultMouseNav::rotMove);
  _fsa->addArc("Rot_Off", "Rot", "Start", offTriggers);
  // in spinDecision override going to the Start state if we have a high enough angular
  // velocity
  _fsa->addArcCallback("Rot_Off", this, &DefaultMouseNav::spinDecision);
  
  // arcs from Spinning
  _fsa->addStateEnterCallback("Spinning", this, &DefaultMouseNav::spinningEnter);
  _fsa->addArc("SpinOff", "Spinning", "Start", onTriggers);
  _fsa->addStateExitCallback("Spinning", this, &DefaultMouseNav::spinOff);

  // arcs for Scaling
  _fsa->addArc("ScaleUp", "Start", "Start",
       splitStringIntoArray(ConfigVal("DefaultMouseNav_ScaleUp", "kbd_PAGEUP_down", false)));
  _fsa->addArcCallback("ScaleUp", this, &DefaultMouseNav::scaleUp);
  _fsa->addArc("ScaleDown", "Start", "Start",
       splitStringIntoArray(ConfigVal("DefaultMouseNav_ScaleUp", "kbd_PAGEDOWN_down", false)));
  _fsa->addArcCallback("ScaleDown", this, &DefaultMouseNav::scaleDown);
  
  _eventMgr->addFsaRef(_fsa);
  _gfxMgr->addDrawCallback(this, &DefaultMouseNav::draw);
}

DefaultMouseNav::~DefaultMouseNav()
{
}

void
DefaultMouseNav::startEnter(VRG3D::EventRef e)
{
  _showIcon = false;
}


void
DefaultMouseNav::initialClick(VRG3D::EventRef e)
{
  Vector2 clickPos = e->get2DData() + _ptrOffset;
  _initialClickPos = clickPos;
  _lastPos = clickPos;

  Vector3 lastHitPoint = _hitPoint;
  _hitPoint = _gfxMgr->screenPointToRoomSpacePoint(_initialClickPos, _hitGeometry);

  if (!_hitGeometry) {
    _hitPoint = Vector3(0,0,0);
  }

/*  Vector3 p1 = _gfxMgr->screenPointToRoomSpaceZEqualsPlane(Vector2(0,0), _hitPoint[2]);
  Vector3 p2 = _gfxMgr->screenPointToRoomSpaceZEqualsPlane(Vector2(0.015,0), _hitPoint[2]);
  _iconRad = (p2-p1).magnitude();
  _iconPos = _gfxMgr->roomPointToVirtualSpace(_hitPoint);
  */
  _showIcon = true;
  _eventMgr->queueTimerEvent(new Event("DefaultMouseNav_SingleClickTimeout"), 1.0);
}

void
DefaultMouseNav::checkMovementForPanOrDolly(VRG3D::EventRef e)
{
  Vector2 newPos = e->get2DData() + _ptrOffset;
  _lastPos = newPos;

  const double panMovementThreshold  = 0.01;
  const double dollyMovementThreshold = 0.01;
  if (fabs(newPos[0] - _initialClickPos[0]) > panMovementThreshold)
    _fsa->jumpToState("Pan");
  else if (fabs(newPos[1] - _initialClickPos[1]) > dollyMovementThreshold)
    _fsa->jumpToState("Dolly");
}


void
DefaultMouseNav::panMove(VRG3D::EventRef e)
{
  Vector2 newPos = e->get2DData() + _ptrOffset;
  Vector3 pRoom1 = _gfxMgr->screenPointToRoomSpaceZEqualsPlane(_lastPos, _hitPoint[2]);
  Vector3 pRoom2 = _gfxMgr->screenPointToRoomSpaceZEqualsPlane(newPos, _hitPoint[2]);
  CoordinateFrame t(pRoom1 - pRoom2);
  _gfxMgr->setRoomToVirtualSpaceFrame(_gfxMgr->getRoomToVirtualSpaceFrame() * t);
  _lastPos = newPos;
}

void
DefaultMouseNav::dollyMove(VRG3D::EventRef e)
{
  Vector2 newPos = e->get2DData() + _ptrOffset;
  // Setup dollyFactor so that if you move the mouse to the bottom of the screen, the point 
  // you clicked on will be right on top of the camera.
  double deltaZToHit    = _hitPoint[2] - _gfxMgr->getCamera()->getCameraPos()[2];
  double deltaYToBottom = _initialClickPos[1] + 1;
  double dollyFactor    = -deltaZToHit / deltaYToBottom;
  Vector3 d(0, 0, dollyFactor * (newPos[1] - _lastPos[1]));
  CoordinateFrame t(d);
  _gfxMgr->setRoomToVirtualSpaceFrame(_gfxMgr->getRoomToVirtualSpaceFrame() * t);
  _lastPos = newPos;
}

void
DefaultMouseNav::rotOn(VRG3D::EventRef e)
{
  _lastRotTime = SynchedSystem::getLocalTime();
  _rotAngularVel = 0.0;
  double rad = 0.55;

  _lastIntersectionPt = Vector3::inf();
  if (_hitGeometry) {
    Vector2 clickPos = e->get2DData() + _ptrOffset;

    Vector3 p1 = _gfxMgr->screenPointToRoomSpaceZEqualsPlane(Vector2(0,0), _hitPoint[2]);
    Vector3 p2 = _gfxMgr->screenPointToRoomSpaceZEqualsPlane(Vector2(0.75,0), _hitPoint[2]);
    rad = G3D::clamp((double)(p2-p1).magnitude(), 0.5, (double)(G3D::abs(_hitPoint[2]) + 0.5));

    _boundingSphere = Sphere(_hitPoint, rad);
  }
  else {
    _boundingSphere = Sphere(Vector3::zero(), rad);
  }
}

void
DefaultMouseNav::rotMove(VRG3D::EventRef e)
{
  Vector2 newPos = e->get2DData() + _ptrOffset;
  _rotAngularVel = 0.0;

  // All coordinates here in RoomSpace
  Vector3 orig = _gfxMgr->getCamera()->getCameraPos();
  Vector3 dir = (_gfxMgr->screenPointToRoomSpaceFilmplane(newPos) - orig).unit();
  Ray r = Ray::fromOriginAndDirection(orig, dir);

  double t = r.intersectionTime(_boundingSphere);
  if ((isFinite(t)) && (t > 0)) {
    Vector3 intersectionPt = orig + t*dir;

    if (_lastIntersectionPt != Vector3::inf()) {
      Vector3 v1 = (_lastIntersectionPt - _boundingSphere.center).unit();
      Vector3 v2 = (intersectionPt - _boundingSphere.center).unit();
      _rotAxis = v1.cross(v2).unit();
      double dot = v1.dot(v2);
      double angle = -aCos(dot);

      double t = SynchedSystem::getLocalTime();
      double deltaT = t - _lastRotTime;
      _lastRotTime = SynchedSystem::getLocalTime();
      if (deltaT != 0.0) {
        _rotAngularVel = angle/deltaT;
      }

      if ((_rotAxis.isFinite()) && (!_rotAxis.isZero())) {
        Matrix3 rot = Matrix3::fromAxisAngle(_rotAxis, angle);
        rot.orthonormalize();
        CoordinateFrame frame = CoordinateFrame(_boundingSphere.center) * 
          CoordinateFrame(rot, Vector3::zero()) *
          CoordinateFrame(-_boundingSphere.center);
        _gfxMgr->setRoomToVirtualSpaceFrame(_gfxMgr->getRoomToVirtualSpaceFrame() * frame);
      }
    }
    _lastIntersectionPt = intersectionPt;
  }
  else {
    _lastIntersectionPt = Vector3::inf();
  }
}



void
DefaultMouseNav::spinDecision(VRG3D::EventRef e)
{
  _showIcon = false;
  //cout << "check for spin: " << _rotAngularVel << endl;
  const double threshold = 0.1;
  if (G3D::abs(_rotAngularVel) > threshold) {    
    _fsa->jumpToState("Spinning");
  }
  else {
    _fsa->jumpToState("Start");
  }
}

void
DefaultMouseNav::spinningEnter(VRG3D::EventRef e)
{
  _animationCallbackID = _gfxMgr->addPoseCallback(this, &DefaultMouseNav::poseForAnimation);
}

void
DefaultMouseNav::poseForAnimation(Array<PosedModel::Ref> &posedModels, const CoordinateFrame &virtualToRoomSpace)
{
  if (_fsa->getCurrentState() == "Spinning") {
    double now = SynchedSystem::getLocalTime();
    double deltaT = now - _lastRotTime;
    _lastRotTime = now;
    double angle = _rotAngularVel * deltaT;
    Matrix3 r = Matrix3::fromAxisAngle(_rotAxis, angle);
    CoordinateFrame f = CoordinateFrame(_boundingSphere.center) * 
      CoordinateFrame(r, Vector3::zero()) *
      CoordinateFrame(-_boundingSphere.center);
    _gfxMgr->setRoomToVirtualSpaceFrame(_gfxMgr->getRoomToVirtualSpaceFrame() * f); 
  }
}

void
DefaultMouseNav::spinOff(VRG3D::EventRef e)
{
  _gfxMgr->removePoseCallback(_animationCallbackID);
}



void
DefaultMouseNav::scaleUp(VRG3D::EventRef e)
{
  _gfxMgr->setRoomToVirtualSpaceScale(0.95 * _gfxMgr->getRoomToVirtualSpaceScale());
}

void
DefaultMouseNav::scaleDown(VRG3D::EventRef e)
{
  _gfxMgr->setRoomToVirtualSpaceScale(1.05 * _gfxMgr->getRoomToVirtualSpaceScale());
}


void 
DefaultMouseNav::draw(RenderDevice *rd, const CoordinateFrame &virtualToRoomSpace)
{
  if (_showIcon) {
    Vector3 p1 = _gfxMgr->screenPointToRoomSpaceZEqualsPlane(Vector2(0,0), _hitPoint[2]);
    Vector3 p2 = _gfxMgr->screenPointToRoomSpaceZEqualsPlane(Vector2(0.015,0), _hitPoint[2]);
    double rad = (p2-p1).magnitude();
    Sphere s(_hitPoint, rad);
    Draw::sphere(s, rd, Color3::black(), Color4::clear());
  }
}




} // namespace
