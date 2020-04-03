#include "TouchBackground.H"

using namespace G3D;

#define MOVE_EPSILON 0.025
#define ROTATION_EPSILON 0.010
#define ROTATION_ANGLE_SCALE 5

//in normalized (0-1) the threshold required for a move to register
#define MOVE_THRESHOLD 0.005

TouchBackground::TouchBackground(GfxMgrRef gfxMgr, Vector3 tableTL, Vector3 tableTR, Vector3 tableBL, Vector3 tableBR) : TouchWidget(gfxMgr, tableTL, tableTR, tableBL, tableBR)
{
  _firstTouchID = -1;
  _secondTouchID = -1;
  //_gfxMgr = gfxMgr;
  _newDst2 = Vector3(0,0,0);
  _mode = 3;
  _importantPoint = ConfigVal("ImportantGeometryPoint", Vector3(0.0,0.0,3), false);
  _visualYPlane = ConfigVal("VisualYPlane", 2.0, false);
  _gfxMgr->setRoomToVirtualSpaceFrame(_gfxMgr->getRoomToVirtualSpaceFrame() * CoordinateFrame(Vector3(0,-(-_importantPoint.y + ConfigVal("VisualYPlane", 2.0, false)),0)));
}

TouchBackground::~TouchBackground()
{
}

bool
TouchBackground::offerTouchDown(int id, Vector2 newPos)
{
  TouchInfo info;
  info.id = id;
  info.pos = convertScreenToRoomCoordinates(newPos);
  info.lastPos = convertScreenToRoomCoordinates(newPos);
  info.screenPos = newPos;
  info.lastScreenPos = newPos;
  info.lastYDirMovement = 0;
  info.lastXDirMovement = 0;
  info.activeWin = -1;

  cout << "Touch at " << newPos.x << ", " << newPos.y << ", " << "\n";
  cout << "Touch at " << info.pos.x << ", " << info.pos.y << ", " << info.pos.z << "\n";
  _touches.append(info);
  if(_firstTouchID == -1) {
    _firstTouchID = id;
  }
  else if(_secondTouchID == -1) {
    _secondTouchID = id;
  }

  return true;
}

void
TouchBackground::resetTouchHistories(int index1, int index2) {
  _touches[index1].lastYDirMovement = 0;
  _touches[index2].lastYDirMovement = 0;
  _touches[index1].lastXDirMovement = 0;
  _touches[index2].lastXDirMovement = 0;
}

bool
TouchBackground::offerTouchMove(int id, Vector2 newPos)
{
  int movingTouchIndex = -1;
  int nonMovingTouchIndex = -1;

  if(id == _firstTouchID) {
    for(int i=0; i<_touches.size(); i++) {
      if(_touches[i].id == id) {
	movingTouchIndex = i;
	//check to make sure it has moved a threshold distance, otherwise it ends up shaky
	if(G3D::abs((_touches[movingTouchIndex].screenPos - newPos).length()) < MOVE_THRESHOLD) {
	  return true;
	}
	else {
	  _touches[movingTouchIndex].lastPos = _touches[movingTouchIndex].pos;
	  _touches[movingTouchIndex].lastScreenPos = _touches[movingTouchIndex].screenPos;
	  _touches[movingTouchIndex].screenPos = newPos;
	  _touches[movingTouchIndex].pos = convertScreenToRoomCoordinates(newPos);
	  _touches[movingTouchIndex].lastYDirMovement += newPos.y - _touches[movingTouchIndex].lastScreenPos.y;
	  _touches[movingTouchIndex].lastXDirMovement += newPos.x - _touches[movingTouchIndex].lastScreenPos.x;
	}
      }
      else if(_touches[i].id == _secondTouchID) {
	nonMovingTouchIndex = i;
      }
    }
  }
  else if(id == _secondTouchID) {
    for(int i=0; i<_touches.size(); i++) {
      if(_touches[i].id == id) {
	movingTouchIndex = i;
	if(G3D::abs((_touches[movingTouchIndex].screenPos - newPos).length()) < MOVE_THRESHOLD) {
	  return true;
	}
	else {
	  _touches[movingTouchIndex].lastPos = _touches[movingTouchIndex].pos;
	  _touches[movingTouchIndex].lastScreenPos = _touches[movingTouchIndex].screenPos;
	  _touches[movingTouchIndex].screenPos = newPos;
	  _touches[movingTouchIndex].pos = convertScreenToRoomCoordinates(newPos);
	  _touches[movingTouchIndex].lastYDirMovement += newPos.y - _touches[movingTouchIndex].lastScreenPos.y;
	  _touches[movingTouchIndex].lastXDirMovement += newPos.x - _touches[movingTouchIndex].lastScreenPos.x;
	}
      }
      else if(_touches[i].id == _firstTouchID) {
	nonMovingTouchIndex = i;
      }
    }
  }
  else {
    return false;
  }


  cout << "moving index = " << movingTouchIndex << "\n";
  cout << "non moving index = " << nonMovingTouchIndex << "\n";
  //two points moving at once
  if(movingTouchIndex != -1 && nonMovingTouchIndex != -1) {
    double yDirMove1 = _touches[movingTouchIndex].lastYDirMovement;
    double yDirMove2 = _touches[nonMovingTouchIndex].lastYDirMovement;
    double xDirMove1 = _touches[movingTouchIndex].lastXDirMovement;
    double xDirMove2 = _touches[nonMovingTouchIndex].lastXDirMovement;


    //mode checking to switch between the various two touch modes
    //check if points are moving in the same y (screen coordinates) direction
    if(((yDirMove1 > 0 && yDirMove2 > 0) || (yDirMove1 < 0 && yDirMove2 < 0)) && (G3D::abs(yDirMove1) > MOVE_EPSILON && G3D::abs(yDirMove2) > MOVE_EPSILON) && _mode != 1)  {
      _mode = 1;
      double min;
      cout << "switching to x rotation\n";
      resetTouchHistories(movingTouchIndex, nonMovingTouchIndex);
    }
    //if they are moving in the same x (screen coordinates) direction
    else if(((xDirMove1 > 0 && xDirMove2 > 0) || (xDirMove1 < 0 && xDirMove2 < 0)) && (G3D::abs(xDirMove1) > MOVE_EPSILON && G3D::abs(xDirMove2) > MOVE_EPSILON) && _mode != 2) {
      _mode = 2;
      cout << "switching to z rotation\n";
      resetTouchHistories(movingTouchIndex, nonMovingTouchIndex);
    }
    //other detects to switch back to y roatation and scaling mode
    else if(((G3D::abs(yDirMove1) < 0.005 && G3D::abs(yDirMove2) > MOVE_EPSILON) ||
	     (G3D::abs(yDirMove2) < 0.005 && G3D::abs(yDirMove1) > MOVE_EPSILON) ||
	     (yDirMove1 > 0 && yDirMove2 < 0 && yDirMove1 - yDirMove2 > MOVE_EPSILON) ||
	     (yDirMove1 < 0 && yDirMove2 > 0 && yDirMove2 - yDirMove1 > MOVE_EPSILON) ||
	     (G3D::abs(xDirMove1) < 0.005 && G3D::abs(xDirMove2) > MOVE_EPSILON) ||
	     (G3D::abs(xDirMove2) < 0.005 && G3D::abs(xDirMove1) > MOVE_EPSILON) ||
	     (xDirMove1 > 0 && xDirMove2 < 0 && xDirMove1 - xDirMove2 > MOVE_EPSILON) ||
	     (xDirMove1 < 0 && xDirMove2 > 0 && xDirMove2 - xDirMove1 > MOVE_EPSILON)) &&
	    _mode != 3) {
      _mode = 3;
      cout << "switching to y rotation/scaling\n";
      resetTouchHistories(movingTouchIndex, nonMovingTouchIndex);
    }
    

    if(_mode == 1) {
      doMode1(yDirMove1,yDirMove2,xDirMove1,xDirMove2,movingTouchIndex,nonMovingTouchIndex);
      return true;
    }
    else if(_mode == 2){
      doMode2(yDirMove1,yDirMove2,xDirMove1,xDirMove2,movingTouchIndex,nonMovingTouchIndex);
      return true;
    }
    
    if(_mode == 3) {
      doMode3(movingTouchIndex,nonMovingTouchIndex);
      return true;
    }
  }
  //single touch
  else if(movingTouchIndex != -1){
    Vector3 src1, src2, dst1, dst2;
    src1 = _touches[movingTouchIndex].lastPos;
    dst1 = _touches[movingTouchIndex].pos;
    CoordinateFrame t(src1 - dst1);
    CoordinateFrame roomToVirtualSpace = _gfxMgr->getRoomToVirtualSpaceFrame();
    _gfxMgr->setRoomToVirtualSpaceFrame(roomToVirtualSpace * t);
    roomToVirtualSpace = _gfxMgr->getRoomToVirtualSpaceFrame();
    cout << "Coordinate origin: " << roomToVirtualSpace.translation.x << " " << roomToVirtualSpace.translation.y << " " << roomToVirtualSpace.translation.z << " \n";
    return true;
  }
  return false;
}

void
TouchBackground::doMode1(double yDirMove1, double yDirMove2, double xDirMove1, double xDirMove2, int movingTouchIndex, int nonMovingTouchIndex) {
  int negativeRot = 1;
  if(yDirMove1 > 0)
    negativeRot = -1;
  double minYtrans = G3D::min(G3D::abs(yDirMove1), G3D::abs(yDirMove2));
  if(G3D::abs(minYtrans) > ROTATION_EPSILON) {
    cout << "rotating by " << minYtrans << "in x direction\n";
    resetTouchHistories(movingTouchIndex, nonMovingTouchIndex);
    
    CoordinateFrame roomToVirtualSpace = _gfxMgr->getRoomToVirtualSpaceFrame();
    Matrix3 rot = Matrix3::fromAxisAngle(Vector3(1,0,0), ROTATION_ANGLE_SCALE * minYtrans * negativeRot);
    Vector3 translation = _gfxMgr->virtualPointToRoomSpace(_importantPoint);
    //translation.x = 0;
    //translation.z = 0;
    CoordinateFrame rotTrans = CoordinateFrame(translation) * CoordinateFrame(rot, Vector3::zero()) * CoordinateFrame(-translation);
    _gfxMgr->setRoomToVirtualSpaceFrame(roomToVirtualSpace * rotTrans);
  }
}

void
TouchBackground::doMode2(double yDirMove1, double yDirMove2, double xDirMove1, double xDirMove2, int movingTouchIndex, int nonMovingTouchIndex) {
  int negativeRot = 1;
  if(xDirMove1 < 0)
    negativeRot = -1;
  double minXtrans = G3D::min(G3D::abs(xDirMove1), G3D::abs(xDirMove2));
  //TODO reset x for every y ticks, don't reset y
  //or just do the rotation before the epsilon check and then reset x????
  if(G3D::abs(minXtrans) > ROTATION_EPSILON) {
    cout << "rotating by " << minXtrans << "in z direction\n";
    resetTouchHistories(movingTouchIndex, nonMovingTouchIndex);
    CoordinateFrame roomToVirtualSpace = _gfxMgr->getRoomToVirtualSpaceFrame();
    Matrix3 rot = Matrix3::fromAxisAngle(Vector3(0,0,1), ROTATION_ANGLE_SCALE * minXtrans * negativeRot);
    Vector3 translation = _gfxMgr->virtualPointToRoomSpace(_importantPoint);
    //translation.x = 0;
    //translation.z = 0;
    CoordinateFrame rotTrans = CoordinateFrame(translation) * CoordinateFrame(rot, Vector3::zero()) * CoordinateFrame(-translation);
    _gfxMgr->setRoomToVirtualSpaceFrame(roomToVirtualSpace * rotTrans);
  }
}

void                 
TouchBackground::doMode3(int movingTouchIndex, int nonMovingTouchIndex) {
  if(G3D::abs(_touches[movingTouchIndex].lastYDirMovement) > (2 * MOVE_EPSILON)) {
    resetTouchHistories(movingTouchIndex, nonMovingTouchIndex);
  }
  if(G3D::abs(_touches[movingTouchIndex].lastXDirMovement) > (2 * MOVE_EPSILON)) {
    resetTouchHistories(movingTouchIndex, nonMovingTouchIndex);
  }
  Vector3 src1, src2, dst1, dst2, src3, dst3;
  src1 = _touches[movingTouchIndex].lastPos;
  dst1 = _touches[movingTouchIndex].pos;
  src2 = _touches[nonMovingTouchIndex].lastPos;
  dst2 = _touches[nonMovingTouchIndex].pos;

  Vector3 oldVec = src1 - dst2;
  Vector3 newVec = dst1 - dst2;
  if(src1 != dst1 && oldVec.unit().dot(newVec.unit()) != 0) {
    cout << "src1: " << src1.x << " " << src1.y << " " << src1.z << " \n";
    cout << "dst1: " << dst1.x << " " << dst1.y << " " << dst1.z << " \n";
    cout << "oldVec: " << oldVec.x << " " << oldVec.y << " " << oldVec.z << " \n";
    cout << "newVec: " << newVec.x << " " << newVec.y << " " << newVec.z << " \n";
    //rotation center is the stationary second point dst2
    Vector3 rotAxis = newVec.cross(oldVec).unit();
    cout << "Rot Axis is: " << rotAxis.x << " " << rotAxis.y << " " << rotAxis.z << "\n";
    double rotAngle = aCos(oldVec.unit().dot(newVec.unit()));
    double scaleFactor = oldVec.magnitude() / newVec.magnitude();
    cout << "Rot Anngle is: " << rotAngle;
    cout << "Scale Factor is: " << scaleFactor << "\n";
    Matrix3 rot = Matrix3::fromAxisAngle(rotAxis, rotAngle);
    rot.orthonormalize();

    CoordinateFrame roomToVirtualSpace = _gfxMgr->getRoomToVirtualSpaceFrame();
    CoordinateFrame rotTrans = CoordinateFrame(dst2) * CoordinateFrame(rot, Vector3::zero()) * CoordinateFrame(-dst2);
    cout << "Translate Before Rot: " << dst2.x << " " << dst2.y << " " << dst2.z << " \n";

    //translate to account for the offset in scaling - there may be an easier way to do this
    //but with how the scaling is setup in graphics manager I can't figure it out
    roomToVirtualSpace = _gfxMgr->getRoomToVirtualSpaceFrame();
    Vector3 virtualOriginInScreenPlane = _gfxMgr->virtualPointToRoomSpace(Vector3(0,0,0));
    virtualOriginInScreenPlane.y = 0.0;
	
    //account for Y offset to keep the imporant point in the viusal plane
    Vector3 virtualImpPoint = _gfxMgr->virtualPointToRoomSpace(Vector3(_importantPoint));
    Vector3 scaledVirtualImpPoint = virtualImpPoint/scaleFactor;
    double yAdjust = scaledVirtualImpPoint.y - _visualYPlane;

    Vector3 newDst2 = dst2 - virtualOriginInScreenPlane;
    cout << "Coordinate origin: " << virtualOriginInScreenPlane.x << " " << virtualOriginInScreenPlane.y << " " << virtualOriginInScreenPlane.z << " \n";
    cout << "dst2 in virtual space: " << newDst2.x << " " << newDst2.y << " " << newDst2.z << " \n";
    newDst2 = newDst2/scaleFactor;
    newDst2 = virtualOriginInScreenPlane + newDst2;
    newDst2.y = yAdjust;
    _newDst2 = newDst2;
    _virtOrig = virtualOriginInScreenPlane;
    CoordinateFrame t(newDst2 - dst2);
    _gfxMgr->setRoomToVirtualSpaceFrame(roomToVirtualSpace * rotTrans * t);
    //_gfxMgr->setRoomToVirtualSpaceFrame(_gfxMgr->getRoomToVirtualSpaceFrame() * t);
    _gfxMgr->setRoomToVirtualSpaceScale(_gfxMgr->getRoomToVirtualSpaceScale() * scaleFactor);
  }
}

bool
TouchBackground::offerTouchUp(int id)
{
  int index = -1;
  int i = 0;
  bool ret = false;

  while ((index == -1) && (i < _touches.size())) {
    if (_touches[i].id == id) {
      index = i;
    }
    i++;
  }

  if (index != -1) {
    _touches.remove(index);
    ret = true;
  }

  if(id == _firstTouchID) {
	  _firstTouchID = -1;
  }
  if(id == _secondTouchID) {
	  _secondTouchID = -1;
  }

  return ret;
}

void
TouchBackground::draw(RenderDevice *rd, const CoordinateFrame &virtualToRoomSpace)
{
  rd->push2D();

  if(ConfigVal("Display", "desktop", false) == "table") {
    for (int i=0;i<_touches.size();i++) {
      Vector2 pos = _touches[i].screenPos * Vector2(rd->width(), rd->height());
      Draw::rect2D(Rect2D::xywh(pos.x-15, pos.y-15, 30, 30), rd, Color3::red());
    }
  }


  /*
  Vector2 xy;
  xy.x = (_newDst2.x - _tableTopLeft.x) / (_tableTopRight.x - _tableTopLeft.x);
  xy.y = (_newDst2.z - _tableTopRight.z) / (_tableBottomRight.z - _tableTopRight.z);
  Vector2 pos = xy * Vector2(rd->width(), rd->height());
  //cout << " drawing at " << pos.x << ", " << pos.y << "\n";
  Draw::rect2DBorder(Rect2D::xywh(pos.x, pos.y, 10, 10), rd, Color3::blue());

  xy.x = (_virtOrig.x - _tableTopLeft.x) / (_tableTopRight.x - _tableTopLeft.x);
  xy.y = (_virtOrig.z - _tableTopRight.z) / (_tableBottomRight.z - _tableTopRight.z);
  pos = xy * Vector2(rd->width(), rd->height());
  //cout << " drawing at " << pos.x << ", " << pos.y << "\n";
  Draw::rect2DBorder(Rect2D::xywh(pos.x, pos.y, 10, 10), rd, Color3::green());
  */
  
  rd->pop2D();
}


