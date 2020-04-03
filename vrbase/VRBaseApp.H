/**
 * \author Daniel Keefe (dfk)
 *
 * \file  VRBaseApp.H
 *
 */

#include "CommonInc.H"
#include "EventMgr.H"
#include "GfxMgr.H"
#include "StringUtils.H"

namespace VRBase {


class VRBaseApp : public VRApp
{
public:

  /// Use this to construct a new VRBaseApp, then call app->run() to start it up
  VRBaseApp(int argc, char **argv) : VRApp() {

    _log = new G3D::Log("log.txt");

    std::string vrSetupStr = "desktop";
    if (argc >= 2) {
      std::string argStr = std::string(argv[1]);
      if (argStr[0] != '-') {
        vrSetupStr = argStr;
      }
    }

    // initialize the VRApp
    init(vrSetupStr, _log);    

    // Startup ConfigVals
    ConfigValMap::map = new ConfigMap(argc, argv, _log, false);

    // Startup the gfx mgr
    _gfxMgr = new GfxMgr(_renderDevice, _cameras[0]);
    G3D::Color3 defaultLtCol = G3D::Color3(0.35f, 0.35f, 0.35f);
    _gfxMgr->getLighting()->ambientTop    = ConfigVal("AmbientTop", defaultLtCol, false);
    _gfxMgr->getLighting()->ambientBottom = ConfigVal("AmbientBottom", defaultLtCol, false);
    _gfxMgr->getLighting()->lightArray.append(G3D::GLight::directional(G3D::Vector3(0,1,1).unit(), G3D::Color3(0.5,0.5,0.5)));
    _gfxMgr->loadTexturesFromConfigVal("LoadTextures", _log);
    
    // Startup the event mgr
    _eventMgr = new EventMgr(_log);
    
    // startup a mouse-based standin for 6DOF trackers if we're on a desktop system
    if (G3D::beginsWith(vrSetupStr, "desk")) {
      _mouseToTracker = new MouseToTracker(_gfxMgr->getCamera(), ConfigVal("MouseToTracker_NumTrackers",2,false));
    }
    
  }

  virtual ~VRBaseApp() {}

  virtual void doUserInput(G3D::Array<VRG3D::EventRef> &events) {
    if (_mouseToTracker.notNull()) {
      G3D::Array<VRG3D::EventRef> newEvents;
      _mouseToTracker->doUserInput(events, newEvents);
      events.append(newEvents);
    }

    for (int i=0;i<events.size();i++) {
      if (events[i]->getName() == "Shutdown") {
        _endProgram = true;
      }
      _eventMgr->queueEvent(events[i]);
    }

    _eventMgr->processEventQueue();
    _gfxMgr->poseFrame();
  }

  virtual void doGraphics(G3D::RenderDevice *rd) {
    _gfxMgr->drawFrame();
    //_gfxMgr->drawStats();
    vrg3dSleepMsecs(ConfigVal("VRBaseApp_SleepTime", 0.0, false));
  }


  GfxMgrRef   getGfxMgr()   { return _gfxMgr; }
  EventMgrRef getEventMgr() { return _eventMgr; }

protected:
  GfxMgrRef         _gfxMgr;
  EventMgrRef       _eventMgr;   
  MouseToTrackerRef _mouseToTracker;
};





} // end namespace

