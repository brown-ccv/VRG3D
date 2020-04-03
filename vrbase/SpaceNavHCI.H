/**
 * \author Daniel Keefe (dfk)
 *
 * \file  SpaceNavHCI.H
 * \brief
 *
 */


#ifndef SPACENAVHCI_H
#define SPACENAVHCI_H

#include "CommonInc.H"
#include "EventMgr.H"
#include "Fsa.H"
#include "GfxMgr.H"

namespace VRBase {

/**
  Intended for use in combination with the Space Navigator.  This
  navigation assumes the model is centered around (0,0,0) in Room
  Space.
*/
class SpaceNavHCI
{
public:  
  SpaceNavHCI(EventMgrRef eventMgr, GfxMgrRef gfxMgr, 
              double rotGain=1.0, double transGain=1.0);
  virtual ~SpaceNavHCI();

  /// Default is 1.0, setting to 0 disables translation
  void setTranslationGain(double gain) { _tGain = gain; }

  /// Default is 1.0, setting to 0 disables rotation
  void setRotationGain(double gain) { _rGain = gain; }

  void updateTrans(VRG3D::EventRef e);
  void updateRot(VRG3D::EventRef e);
  void scaleUp(VRG3D::EventRef e);
  void scaleDown(VRG3D::EventRef e);

protected:
  EventMgrRef     _eventMgr;
  FsaRef          _fsa;
  GfxMgrRef       _gfxMgr;
  double          _rGain;
  double          _tGain;
};

}
#endif
