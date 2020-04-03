#ifndef TOUCHWIDGET_H
#define TOUCHWIDGET_H

#include "CommonInc.H"
#include "ConfigVal.H"
#include "GfxMgr.H"

using namespace VRBase;

typedef G3D::ReferenceCountedPointer<class TouchWidget> TouchWidgetRef;

class TouchInfo
{
public:
  int id;
  G3D::Vector3 pos;
  G3D::Vector3 lastPos;
  G3D::Vector2 screenPos;
  G3D::Vector2 lastScreenPos;
  double lastYDirMovement;
  double lastXDirMovement;
  int activeWin;
  bool isValid;
  
  enum Hand{
    RIGHT_HAND=0,
    LEFT_HAND
  };
  
  Hand correspondingHand;
};

class WidgetLayerListener
{
public:
  virtual ~WidgetLayerListener() {};
  virtual void moveToTop(TouchWidgetRef thisWidg) = 0;
  virtual void moveToBottom(TouchWidgetRef thisWidg) = 0;
  virtual void moveAbove(TouchWidgetRef thisWidg, TouchWidgetRef refWidg) = 0;
  virtual void moveBelow(TouchWidgetRef thisWidg, TouchWidgetRef refWidg) = 0;
};

class WidgetFactory
{
public:
  virtual ~WidgetFactory() {};
  virtual void createWidget(TouchWidgetRef widgToSpawn) = 0;
  virtual void deleteWidget(TouchWidgetRef widgToDelete) = 0;
};

class TouchWidget : public G3D::ReferenceCountedObject
{
public:
  TouchWidget(GfxMgrRef gfxMgr, G3D::Vector3 tableTL, G3D::Vector3 tableTR, G3D::Vector3 tableBL, G3D::Vector3 tableBR);
  TouchWidget(GfxMgrRef gfxMgr, WidgetLayerListener* layerListen, WidgetFactory* widgetFactory, G3D::Vector3 tableTL, G3D::Vector3 tableTR, G3D::Vector3 tableBL, G3D::Vector3 tableBR);

  virtual ~TouchWidget() { };

  virtual bool offerTouchDown(int id, G3D::Vector2 pos);
  virtual bool offerTouchUp(int id);
  virtual bool offerTouchMove(int id, G3D::Vector2 pos);
  virtual void draw(G3D::RenderDevice *rd, const G3D::CoordinateFrame &virtualToRoomSpace);
  virtual G3D::Rect2D getInteractRect() { return _interactRect; };
  virtual G3D::Vector2 getInteractCenter() { return _interactCenter; };
  virtual void moveTo(G3D::Vector2 location) { };
  virtual void updateRects() { };
  //additional function that is called once perframe to do some clean up if necessary
  virtual void perFrame() { };

  void show();
  void hide();
  void setID(int id);
  int getID();
  void setSurroundWidgets(G3D::Array<TouchWidgetRef> widgets) { _surroundWidgets = widgets; };
  void setContainedWidgets(G3D::Array<TouchWidgetRef> widgets) { _containedWidgets = widgets; };
  void setInteractInfo(G3D::Rect2D rect, G3D::Vector2 center) {
    _interactRect = rect;
    _interactCenter = center;
    _interactChanged = true;
  };
  bool areBoundsChanged() { return _interactChanged; };
  void markInteractUpdate() { _interactChanged = false; };
  bool isHidden() { return _hidden; };
  void setLayerListener(WidgetLayerListener* layerListen) { _layerListener = layerListen; };
  void setWidgetFactory(WidgetFactory* widgFactory) { _widgetFactory = widgFactory; };
  
protected:
  WidgetLayerListener* _layerListener;
  WidgetFactory*       _widgetFactory;
  int                  _widgetID;
  int                  _drawID;
  GfxMgrRef            _gfxMgr;
  bool                 _hidden;
  G3D::Vector3              _tableTopLeft;
  G3D::Vector3              _tableTopRight;
  G3D::Vector3              _tableBottomLeft;
  G3D::Vector3              _tableBottomRight;
  int                  _lastTouchTime; //in miliseconds

  G3D::Array<TouchWidgetRef> _surroundWidgets;
  G3D::Array<TouchWidgetRef> _containedWidgets;
  G3D::Rect2D                _interactRect;
  G3D::Vector2               _interactCenter;
  bool                  _interactChanged;

  G3D::Vector3 convertScreenToRoomCoordinates(G3D::Vector2 xy);
  G3D::Vector2 convertRoomToScreenCoordinates(G3D::Vector3 room);
};

template<> struct BoundsTrait<TouchWidgetRef> {
  static void getBounds(const TouchWidgetRef& s, G3D::AABox& out) { 
    G3D::Rect2D rect = s->getInteractRect();
    out = G3D::AABox(G3D::Vector3(rect.x0(),rect.y0(),0),G3D::Vector3(rect.x1(),rect.y1(),0)); 
  }
};

template<> struct PositionTrait<TouchWidgetRef> {
  static void getPosition(const TouchWidgetRef& s, G3D::Vector3& out) { 
    G3D::Vector2 cent = s->getInteractCenter();
    out = G3D::Vector3(cent.x,cent.y,0);
  }
};

template<> struct HashTrait<TouchWidgetRef> {
  static size_t hashCode(const TouchWidgetRef& key) { 
    return key->getID();
  }
};

template<> struct EqualsTrait<TouchWidgetRef> {
  static size_t equals(const TouchWidgetRef& a, const TouchWidgetRef& b) { 
    return a == b;
  }
};

#endif
