#include "TouchManager.H"

using namespace G3D;

TouchManager::TouchManager(GfxMgrRef gfxMgr) 
{
  _gfxMgr = gfxMgr;
  _nextWidgetID = 0;
  _gfxMgr->addDrawCallback(this, &TouchManager::draw);
  _updateThreshold = ConfigVal("TouchManagerUpdateThreshold", 0.5, false);
  _lastFrameTime = G3D::System::time();
  _updateTimer = 0.0;
}

TouchManager::~TouchManager()
{
  
}

void
TouchManager::touchDown(int id, Vector2 newPos)
{
  for(int i = 0; i < _widgetList.size(); i++) {
    if(_widgetList[i]->offerTouchDown(id, newPos)) {
      break;
    }
  }

  TouchInfo info;
  info.id = id;
  info.screenPos = newPos;
  info.lastScreenPos = newPos;
  info.lastYDirMovement = 0;
  info.lastXDirMovement = 0;
  info.activeWin = -1;

  _touches.append(info);
}

void 
TouchManager::touchUp(int id) 
{
  for(int i = 0; i < _widgetList.size(); i++) {
    if(_widgetList[i]->offerTouchUp(id)) {
      break;
    }
  }

  int index = -1;
  int i = 0;
  while ((index == -1) && (i < _touches.size())) {
    if (_touches[i].id == id) {
      index = i;
    }
    i++;
  }

  if (index != -1) {
    _touches.remove(index);
  }
}

void 
TouchManager::touchMove(int id, Vector2 newPos) 
{
  int index = -1;
  int i = 0;

  while ((index == -1) && (i < _touches.size())) {
    if (_touches[i].id == id) {
      index = i;
    }
    i++;
  }

  if(index != -1) {
    //filter redundant move events
    if(newPos != _touches[index].screenPos) {
      _touches[index].lastScreenPos = _touches[index].screenPos;
      _touches[index].screenPos = newPos;
    }

    for(int i = 0; i < _widgetList.size(); i++) {
      if(_widgetList[i]->offerTouchMove(id, newPos)) {
	break;
      }
    }
  }
}

void
TouchManager::addWidget(TouchWidgetRef widgToAdd) 
{
  widgToAdd->setLayerListener(this);
  widgToAdd->setWidgetFactory(this);
  _widgetList.append(widgToAdd);
  widgToAdd->setID(_nextWidgetID);
  widgToAdd->show();
  _nextWidgetID++;
  updateWidgetInteractions(true);
}

void
TouchManager::removeWidget(TouchWidgetRef widgToRemove) 
{
  for(int i = 0; i < _widgetList.size(); i++) {
    if(widgToRemove->getID() == _widgetList[i]->getID()) {
      _widgetList.remove(i);
      _widgetPointTree.remove(widgToRemove);
      _widgetBoxTree.remove(widgToRemove);
      break;
    }
  }
  widgToRemove->setLayerListener(NULL);
  widgToRemove->setWidgetFactory(NULL);
  updateWidgetInteractions(true);
}

void
TouchManager::moveWidgetToFront(int widgetID) {
  for(int i = 0; i< _widgetList.size(); i++) {
    if(widgetID == _widgetList[i]->getID()) {
      TouchWidgetRef tempWidg = _widgetList[i];
      _widgetList.remove(i);
      _widgetList.insert(0, tempWidg);
    }
  }
}

void
TouchManager::perFrame() {
  double curTime = G3D::System::time();
  double timeDiff = curTime = _lastFrameTime;
  _updateTimer += timeDiff;
  _lastFrameTime = curTime;
  if(_updateTimer > _updateThreshold) {
    updateWidgetInteractions();
    _updateTimer = 0.0;
  }
  for(int i = 0; i < _widgetList.size(); i++) {
    _widgetList[i]->perFrame();
  }
}


void
TouchManager::draw(RenderDevice *rd, const CoordinateFrame &virtualToRoomSpace)
{
  rd->push2D();

  if(ConfigVal("Display", "desktop", false) == "table") {
    for (int i=0;i<_touches.size();i++) {
      Vector2 pos = _touches[i].screenPos * Vector2(rd->width(), rd->height());
      Draw::rect2D(Rect2D::xywh(pos.x-2, pos.y-2, 4, 4), rd, Color3::red());
    }
  }
  
  rd->pop2D();

  //call draw function of each widget in the proper order
  for(int i = _widgetList.size() - 1; i >= 0; i--) {
    if(!_widgetList[i]->isHidden()) {
      _widgetList[i]->draw(rd, virtualToRoomSpace);
    }
  }
}

void
TouchManager::updateWidgetInteractions(bool forceUpdate)
{
  bool someChange = forceUpdate;
  for(int i = 0; i < _widgetList.size(); i++) {
    if(!_widgetBoxTree.contains(_widgetList[i])) {
      _widgetBoxTree.insert(_widgetList[i]);
      _widgetPointTree.insert(_widgetList[i]);
      someChange = true;
    }
    else {
      if(_widgetList[i]->areBoundsChanged()) {
	_widgetBoxTree.remove(_widgetList[i]);
	_widgetBoxTree.insert(_widgetList[i]);
	_widgetPointTree.remove(_widgetList[i]);
	_widgetPointTree.insert(_widgetList[i]);
	_widgetList[i]->markInteractUpdate();
	someChange = true;
      }
    }
  }

  if(someChange) {
    for(int i = 0; i < _widgetList.size(); i++) {
      Vector2 cent = _widgetList[i]->getInteractCenter();
      Sphere testSphere = Sphere(Vector3(cent.x,cent.y,0),1);
      Rect2D testRect = _widgetList[i]->getInteractRect();
      Array<TouchWidgetRef> intersects;
      Array<double> dists;
      _widgetBoxTree.getIntersectingMembers(testSphere, intersects);
      for(int j = 0; j < intersects.size(); j++) {
	Vector2 hitCenter = intersects[j]->getInteractCenter();
	Rect2D hitRect = intersects[j]->getInteractRect();
	dists.append((cent - intersects[j]->getInteractCenter()).length());
      }
      Array<TouchWidgetRef> sortedIntersects;
      while(intersects.size() != 0) {
	double minDist = finf();
	int minInd;
	for(int j = 0; j < intersects.size(); j++) {
	  if(dists[j] < minDist) {
	    minInd = j;
	    minDist = dists[j];
	  }
	}
	sortedIntersects.append(intersects[minInd]);
	intersects.remove(minInd);
	dists.remove(minInd);
      }
      _widgetList[i]->setSurroundWidgets(sortedIntersects);
    }

    for(int i = 0; i < _widgetList.size(); i++) {
      Vector2 cent = _widgetList[i]->getInteractCenter();
      Rect2D rect = _widgetList[i]->getInteractRect();
      AABox testBox = AABox(Vector3(rect.x0(),rect.y0(),0),Vector3(rect.x1(),rect.y1(),0)); 
      Array<TouchWidgetRef> intersects;
      Array<double> dists;
      _widgetPointTree.getIntersectingMembers(testBox, intersects);
      for(int j = 0; j < intersects.size(); j++) {
	Vector2 hitCenter = intersects[j]->getInteractCenter();
	Rect2D hitRect = intersects[j]->getInteractRect();
	dists.append((cent - intersects[j]->getInteractCenter()).length());
      }
      Array<TouchWidgetRef> sortedIntersects;
      while(intersects.size() != 0) {
	double minDist = finf();
	int minInd;
	for(int j = 0; j < intersects.size(); j++) {
	  if(dists[j] < minDist) {
	    minInd = j;
	    minDist = dists[j];
	  }
	}
	sortedIntersects.append(intersects[minInd]);
	intersects.remove(minInd);
	dists.remove(minInd);
      }
      _widgetList[i]->setContainedWidgets(sortedIntersects);
    }
  }
}

void 
TouchManager::moveToTop(TouchWidgetRef thisWidg)
{
  moveWidgetToFront(thisWidg->getID());
}

void 
TouchManager::moveToBottom(TouchWidgetRef thisWidg)
{
}

void 
TouchManager::moveAbove(TouchWidgetRef thisWidg, TouchWidgetRef refWidg)
{
}

void 
TouchManager::moveBelow(TouchWidgetRef thisWidg, TouchWidgetRef refWidg)
{
}

void 
TouchManager::createWidget(TouchWidgetRef widgToSpawn)
{
  addWidget(widgToSpawn);
}

void 
TouchManager::deleteWidget(TouchWidgetRef widgToDelete)
{
  removeWidget(widgToDelete);
}
