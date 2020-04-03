#ifndef TOUCHMANAGER_H
#define TOUCHMANAGER_H

#include "TouchWidget.H"

typedef G3D::ReferenceCountedPointer<class TouchManager> TouchManagerRef;

class TouchManager : public G3D::ReferenceCountedObject, public WidgetLayerListener, public WidgetFactory
{
public:
  TouchManager(GfxMgrRef gfxMgr);
  ~TouchManager();

  void touchDown(int id, G3D::Vector2 pos);
  void touchUp(int id);
  void touchMove(int id, G3D::Vector2 pos);
  void addWidget(TouchWidgetRef widgToAdd);
  void removeWidget(TouchWidgetRef widgToRemove);
  void moveWidgetToFront(int widgetID);
  void draw(G3D::RenderDevice *rd, const G3D::CoordinateFrame &virtualToRoomSpace);
  
  //calls a perFrame method for each widget, allowing for clean up code
  void perFrame();

  void moveToTop(TouchWidgetRef thisWidg);
  void moveToBottom(TouchWidgetRef thisWidg);
  void moveAbove(TouchWidgetRef thisWidg, TouchWidgetRef refWidg);
  void moveBelow(TouchWidgetRef thisWidg, TouchWidgetRef refWidg);

  void createWidget(TouchWidgetRef widgToSpawn);
  void deleteWidget(TouchWidgetRef widgToDelete);

  G3D::Array<TouchWidgetRef> getAllWidgets() { return _widgetList; };

protected:
  G3D::Array<TouchWidgetRef>       _widgetList;
  GfxMgrRef                   _gfxMgr;
  int                         _nextWidgetID;
  G3D::Array<TouchInfo>            _touches;
  double _updateTimer;
  double _updateThreshold;
  double _lastFrameTime;

  G3D::KDTree<TouchWidgetRef>               _widgetBoxTree;
  G3D::PointKDTree<TouchWidgetRef>          _widgetPointTree;
  void updateWidgetInteractions(bool forceUpdate=false);
};



#endif
