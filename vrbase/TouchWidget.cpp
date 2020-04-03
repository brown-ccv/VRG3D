#include "TouchWidget.H"

#include "CommonInc.H"
#include "ConfigVal.H"
#include "GfxMgr.H"

using namespace G3D;

TouchWidget::TouchWidget(GfxMgrRef gfxMgr, Vector3 tableTL, Vector3 tableTR, Vector3 tableBL, Vector3 tableBR) {
  _layerListener = NULL;
  _widgetFactory = NULL;
  _gfxMgr = gfxMgr;
  _tableTopLeft = tableTL;
  _tableTopRight = tableTR;
  _tableBottomLeft = tableBL;
  _tableBottomRight = tableBR;
  _interactRect = Rect2D::xywh(-999999,-999999,0,0);
  _interactCenter = Vector2(999999,999999);
  _interactChanged = true;
  _drawID = -1;
  _widgetID = -1;
  _hidden = false;
}

TouchWidget::TouchWidget(GfxMgrRef gfxMgr, WidgetLayerListener* layerListen, WidgetFactory* widgetFactory, Vector3 tableTL, Vector3 tableTR, Vector3 tableBL, Vector3 tableBR) {
  _layerListener = layerListen;
  _widgetFactory = widgetFactory;
  _gfxMgr = gfxMgr;
  _tableTopLeft = tableTL;
  _tableTopRight = tableTR;
  _tableBottomLeft = tableBL;
  _tableBottomRight = tableBR;
  _interactRect = Rect2D::xywh(-999999,-999999,0,0);
  _interactCenter = Vector2(999999,999999);
  _interactChanged = true;
  _drawID = -1;
  _widgetID = -1;
  _hidden = false;
}

bool
TouchWidget::offerTouchDown(int id, Vector2 pos) 
{
  return false;
}

bool
TouchWidget::offerTouchUp(int id)
{
  return false;
}

bool
TouchWidget::offerTouchMove(int id, Vector2 pos)
{
  return false;
}

void
TouchWidget::draw(RenderDevice *rd, const CoordinateFrame &virtualToRoomSpace)
{
}

void
TouchWidget::show() 
{
  //If a widget ID has been set, the manager will take care of drawing this
  //Otherwise use the graphics mananger
  if(_widgetID == -1) {
    _drawID = _gfxMgr->addDrawCallback(this, &TouchWidget::draw);
    _hidden = false;
  }
  else {
    _hidden = false;
  }
}

void
TouchWidget::hide()
{
  if(_widgetID == -1) {
    if(_drawID != -1) {
      _gfxMgr->removeDrawCallback(_drawID);
      _drawID = -1;
    }
  }
  else {
    _hidden = true;
  }
}

void
TouchWidget::setID(int id)
{
  _widgetID = id;
}

int
TouchWidget::getID() 
{
  return _widgetID;
}

Vector3
TouchWidget::convertScreenToRoomCoordinates(Vector2 xy) 
{
  Vector3 xvec = _tableTopRight - _tableTopLeft;
  Vector3 yvec = _tableBottomRight - _tableTopRight;
  Vector3 p = _tableTopLeft + (xy.x * xvec) + (xy.y * yvec);
  return p;
}

Vector2
TouchWidget::convertRoomToScreenCoordinates(Vector3 room) 
{
  Vector3 xvec = _tableTopRight - _tableTopLeft;
  Vector3 yvec = _tableBottomRight - _tableTopRight;
  Vector2 scaledPt = Vector2((room.x - _tableTopLeft.x) / xvec.x, (room.z - _tableTopLeft.z) / yvec.z);
  return scaledPt;
}
