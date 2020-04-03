/**
 * /author Dan Keefe (dfk)
 *
 * /file  Markers.H
 * /brief 
 *
 */ 

#ifndef MARKERS_H
#define MARKERS_H

#include <CommonInc.H>
#include "GfxMgr.H"

namespace VRBase {

typedef G3D::ReferenceCountedPointer<class Markers> MarkersRef;
/**
*/
class Markers : public G3D::ReferenceCountedObject
{ 
public: 
  Markers(GfxMgrRef gfxMgr);
  virtual ~Markers();

  void addMarkersFromFile(const std::string &filename, G3D::Color3 markerColor, double markerSize,
                          double scaleBy, const G3D::CoordinateFrame &transformBy);

  /// Reads an older bat marker data file format
  void addMarkersFromBatDataFile(const std::string &filename, G3D::Color3 markerColor, double markerSize,
                                 double scaleBy, const G3D::CoordinateFrame &transformBy, int timestepInc);

  /// Reads the latest (as of Oct. 2006) bat motion data using the MarkersMotion API
  void addMarkersFromMarkersMotionFile(const std::string &filename, G3D::Color3 markerColor, double markerSize,
                                       double scaleBy, const G3D::CoordinateFrame &transformBy, int timestepInc);


  void draw(G3D::RenderDevice *rd, const G3D::CoordinateFrame &virtualToRoomSpace);

  void displayAllMarkerSets() { _activeSet = -1; }
  void displayOneSetOnly(int setNum) { _activeSet = setNum; }
  void setHidden(bool h) { _hidden = h; }
  int  getNumMarkerSets() { return _markers.size(); }
  int  getNumMarkers(int set) { return _markers[set].size(); }
  G3D::Vector3 getMarkerPosition(int set, int marker) { return _markers[set][marker]; }

private:
  GfxMgrRef               _gfxMgr;
  G3D::Array< G3D::Array<G3D::Vector3> > _markers;
  G3D::Array< G3D::Color3 >         _colors;
  G3D::Array< double >         _sizes;
  int                     _activeSet;
  bool                    _hidden;
};

} // end namespace

#endif



