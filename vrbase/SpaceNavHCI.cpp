#include "SpaceNavHCI.H"

#include "ConfigVal.H"
#include "StringUtils.H"


namespace VRBase {

using namespace G3D;

SpaceNavHCI::SpaceNavHCI(EventMgrRef eventMgr, GfxMgrRef gfxMgr,
                         double rotGain, double transGain)
{
  _eventMgr = eventMgr;
  _gfxMgr = gfxMgr;
  _rGain = rotGain;
  _tGain = transGain;

  _fsa = new Fsa("SpaceNavHCI FSA");
  _fsa->addState("Start");

  _fsa->addArc("ScaleUp", "Start", "Start", splitStringIntoArray("SpaceNav_Btn1_down"));
  _fsa->addArcCallback("ScaleUp", this, &SpaceNavHCI::scaleUp);
  _fsa->addArc("ScaleDown", "Start", "Start", splitStringIntoArray("SpaceNav_Btn2_down"));
  _fsa->addArcCallback("ScaleDown", this, &SpaceNavHCI::scaleDown);

  _fsa->addArc("Trans", "Start", "Start", splitStringIntoArray("SpaceNav_Trans"));
  _fsa->addArcCallback("Trans", this, &SpaceNavHCI::updateTrans);
  _fsa->addArc("Rot", "Start", "Start", splitStringIntoArray("SpaceNav_Rot"));
  _fsa->addArcCallback("Rot", this, &SpaceNavHCI::updateRot);

 _eventMgr->addFsaRef(_fsa);
}

SpaceNavHCI::~SpaceNavHCI()
{
}


void
SpaceNavHCI::updateRot(VRG3D::EventRef e)
{
  if (_rGain != 0.0) {
    Vector3 raw = e->get3DData();
    Vector3 flipped(-raw[0], raw[2], -raw[1]);
    double maxValue = 1.0;
    Vector3 normalized = flipped / maxValue;
    //Vector3 rnorm = _rGain * rangles;
    // This gives us something like: max pressure yields an incremental
    // rotation of 2.5 degrees each frame
    Vector3 rangles = _rGain * normalized * toRadians(2.5);
    //cout << "rangles = " << rangles << endl;

    Matrix3 rot = Matrix3::fromEulerAnglesXYZ(rangles[0], rangles[1], rangles[2]);
    rot.orthonormalize();
    CoordinateFrame frame = CoordinateFrame(rot, Vector3::zero());
    _gfxMgr->setRoomToVirtualSpaceFrame(_gfxMgr->getRoomToVirtualSpaceFrame() * frame);
  }
}


void
SpaceNavHCI::updateTrans(VRG3D::EventRef e)
{
  if (_tGain != 0.0) {
    Vector3 raw = e->get3DData();
    Vector3 flipped(-raw[0], raw[2], -raw[1]);
    double maxValue = 1.0;
    Vector3 normalized = flipped / maxValue;
    // This gives us something like: max pressure yields an incremental
    // translation of 0.01 units each frame
    Vector3 trans = _tGain * 0.01 * normalized;
    //cout << "trans = " << trans << endl;
    CoordinateFrame t(trans);
    _gfxMgr->setRoomToVirtualSpaceFrame(_gfxMgr->getRoomToVirtualSpaceFrame() * t);
  }
}


void
SpaceNavHCI::scaleUp(VRG3D::EventRef e)
{
  _gfxMgr->setRoomToVirtualSpaceScale(0.95 * _gfxMgr->getRoomToVirtualSpaceScale());
}

void
SpaceNavHCI::scaleDown(VRG3D::EventRef e)
{
  _gfxMgr->setRoomToVirtualSpaceScale(1.05 * _gfxMgr->getRoomToVirtualSpaceScale());
}



} // namespace
