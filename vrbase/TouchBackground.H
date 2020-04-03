#ifndef TOUCHBACKGROUND_H
#define TOUCHBACKGROUND_H

#include "TouchWidget.H"

typedef G3D::ReferenceCountedPointer<class TouchBackground> TouchBackgroundRef;

class TouchBackground : public TouchWidget
{
public:
  TouchBackground(GfxMgrRef gfxMgr, G3D::Vector3 tableTL, G3D::Vector3 tableTR, G3D::Vector3 tableBL, G3D::Vector3 tableBR);
  ~TouchBackground();

  bool offerTouchDown(int id, G3D::Vector2 pos);
  bool offerTouchUp(int id);
  bool offerTouchMove(int id, G3D::Vector2 pos);
  void draw(G3D::RenderDevice *rd, const G3D::CoordinateFrame &virtualToRoomSpace);

protected:
  G3D::Array<TouchInfo>     _touches;
  //GfxMgrRef            _gfxMgr;
  int                  _firstTouchID;
  int                  _secondTouchID;
  G3D::Vector3              _newDst2;
  G3D::Vector3 	       _virtOrig;
  int                  _mode;  //1 = x rotation, 2 = z rotation, 3 = scaling/z roation
  G3D::Vector3              _importantPoint;
  double               _visualYPlane;

  void                 resetTouchHistories(int index1, int index2);
  void                 doMode1(double yDirMove1, double yDirMove2, double xDirMove1, double xDirMove2, int movingTouchIndex, int nonMovingTouchIndex);
  void                 doMode2(double yDirMove1, double yDirMove2, double xDirMove1, double xDirMove2, int movingTouchIndex, int nonMovingTouchIndex);
  void                 doMode3(int movingTouchIndex, int nonMovingTouchIndex);
  
};

#endif
