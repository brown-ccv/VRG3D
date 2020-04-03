//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

#include  "VRApp.h"
#include  "CheckerboardStencil.h"
#include  "HorizontalStencil.h"
#include  "ConfigMap.h"
#include  "EventNet.h"
#include  "SynchedSystem.h"

#ifdef USE_GLUT
   #include  "GlutWindow.h"
#endif

#ifdef USE_OMNIMAP
#include "omnimap.h"
#include <gl\glew.h>
#endif


#include <iostream>
using namespace std;
using namespace G3D;


// if the program is a client and the server is running on the same machine, then
// it needs to sleep a bit to allow the server to have some cycles
#define CLIENT_SLEEP 0.0005

#ifdef USE_OMNIMAP
//Declare a pointer to the OmniMap for OpenGL renderer.
OmniMap*               _omnimapLib = NULL;
#endif

namespace VRG3D {

G3D::GMutex  initVRAppMutex;


static void gWindowLoopCallback(void *ptrToVRApp)
{
  VRApp *app = (VRApp*)ptrToVRApp;
  app->oneFrame();

  if (app->getReadyToEndProgram())
  { app->getRenderDevice()->window()->popLoopBody();
    // bug? the pop above doesn't seem to work

    exit(0);
  }
}


Array<std::string> VRApp::_dataFilePaths;

VRApp::VRApp(std::string  share) :
  _log(NULL),
  _gwindow(NULL),
  _renderDevice(NULL),
  _endProgram(false),
  _activeCameraNum(0),
  _curMousePos(Vector2::zero()),   //+++
  _frameCounter(0),
  _isAClusterClient(false),
  _clearColor(Color4(0,0,0,1)),
  _adjustTileOnResizeEvent(true),
  _blueLineStereoLeftFrame(true),
  _clusterServerName("127.0.0.1")  // localhost

{
  std::string  replace;

  if (_dataFilePaths.size() == 0)
  {
    // These paths are relative to the PWD.
    // The PWD must be established before launching.
    //
    // The directories should be checked to see if they exist.
    //
    _dataFilePaths.append("");
    _dataFilePaths.append("share/");
    _dataFilePaths.append("share/vrsetup/");
    _dataFilePaths.append("vrsetup/");
    _dataFilePaths.append("../share/");
    _dataFilePaths.append("../share/vrsetup/");

    if ( share != "" )
    {  _dataFilePaths.append( share );
       _dataFilePaths.append( share + "/vrsetup/" );
       /// cout << "+++++ share = " << share << endl;
    }


    //  These paths are specific to developers at Brown.
    const char *g = getenv( "G" );

    if ( g  &&  getenv( "GARCH" ))
    {  if (( g[ strlen( g ) - 1 ] == '\\' )  ||
           ( g[ strlen( g ) - 1 ] == '/' ))
       {
          char  put[ 1000 ];
          strcpy( put, (char *)"G=" );
          strncat( put, g, strlen( g ) - 1 );

          putenv( put );
       }

       replace = replaceEnvVars("$(G)/install_$(GARCH)/share/vrg3d/");

       if ( replace != "" )
       {  _dataFilePaths.append( replace );
       }

       replace = replaceEnvVars("$(G)/install_$(GARCH)/share/vrg3d/vrsetup/");

       if ( replace != "" )
       {  _dataFilePaths.append( replace );
       }
    }

    //Path specific data file locations for UMN installs
    replace = replaceEnvVars("$(G)/src/SFVRG3D/vrg3dsvn-latest/vrg3d/share/");

    if ( replace != "" )
    {  _dataFilePaths.append( replace );
    }

    replace = replaceEnvVars("$(G)/src/SFVRG3D/vrg3dsvn-latest/vrg3d/share/vrsetup/");

    if ( replace != "" )
    {  _dataFilePaths.append( replace );
    }
  }
}


VRApp::~VRApp()  {}


void  VRApp::init(const std::string &vrSetup, Log *appLog)
{
  initVRAppMutex.lock();

  if (appLog == NULL)
  {   _log = new Log("log.txt");
  }
  else
  {   _log = appLog;
  }

  GWindow::Settings gwinsettings;
  bool hideMouse = false;
  double nearclip = 0.01;
  double farclip = 100.0;
  DisplayTile::TileRenderType renderType = DisplayTile::TILE_MONO;
  CoordinateFrame initialHeadFrame = CoordinateFrame(Vector3(0,0,1));
  Vector3 topLeft  = Vector3(-0.65, 0.5, 0.0);
  Vector3 topRight = Vector3( 0.65, 0.5, 0.0);
  Vector3 botLeft  = Vector3(-0.65,-0.5, 0.0);
  Vector3 botRight = Vector3( 0.65,-0.5, 0.0);
  int viewportX = 0;
  int viewportY = 0;
  int viewportW = 0;
  int viewportH = 0;
  bool tileAutoAdjustToResolution = false;
  bool setupOmnimap = false;
  string omnimapConfig;
  string omnimapLuaRes;

  std::string configfile;

  if (vrSetup != "")
  {  configfile = findVRG3DDataFile(vrSetup + ".vrsetup");
  }

  if ((configfile != "") && (FileSystem::exists(configfile)))
  { ConfigMapRef map = new ConfigMap(configfile, _log);

    int iFalse = 0;
    _isAClusterClient = map->get("ClusterClient", iFalse);
    ///_clusterServerName = map->get("ClusterServerHost", "myserver.cs.brown.edu");
    _clusterServerName = map->get("ClusterServerHost", "127.0.0.1");

    std::string devCfgFile = map->get("InputDevicesFile", "");

    if (devCfgFile != "")
    { setupInputDevicesFromConfigFile( findVRG3DDataFile(devCfgFile),
                                       _log, _inputDevices );
    }

    if ((map->get("WindowWidth", "1280") == "max") ||
        (map->get("WindowHeight", "1024") == "max"))
    {
        VRApp::initScreenSize( gwinsettings );
    }
    else
    {  gwinsettings.width      = map->get("WindowWidth",  gwinsettings.width);
       gwinsettings.height     = map->get("WindowHeight", gwinsettings.height);
    }

    gwinsettings.x             = map->get("WindowX", gwinsettings.x);
    gwinsettings.y             = map->get("WindowY", gwinsettings.y);
    gwinsettings.center        = map->get("WindowCenter", gwinsettings.center);
    gwinsettings.rgbBits       = map->get("WindowRGBBits", gwinsettings.rgbBits);
    gwinsettings.alphaBits     = map->get("WindowAlphaBits", gwinsettings.alphaBits);
    gwinsettings.depthBits     = map->get("WindowDepthBits", gwinsettings.depthBits);
    gwinsettings.stencilBits   = map->get("WindowStencilBits", gwinsettings.stencilBits);
    gwinsettings.msaaSamples   = map->get("WindowFSAASamples", gwinsettings.msaaSamples);
    gwinsettings.fullScreen    = map->get("WindowFullScreen", gwinsettings.fullScreen);
    //gwinsettings.asychronous   = map->get("WindowAsychronous", gwinsettings.asychronous);
    gwinsettings.asynchronous  = map->get("WindowAsynchronous", gwinsettings.asynchronous);
    gwinsettings.stereo        = map->get("WindowStereo", gwinsettings.stereo);
    gwinsettings.refreshRate   = map->get("WindowRefreshRate", gwinsettings.refreshRate);
    gwinsettings.resizable     = map->get("WindowResizable", gwinsettings.resizable);
    gwinsettings.allowMaximize = map->get("WindowAllowMaximize", gwinsettings.allowMaximize);
    gwinsettings.framed        = map->get("WindowFramed", gwinsettings.framed);
    gwinsettings.caption       = map->get("WindowCaption", gwinsettings.caption);

    if (!map->get("WindowMouseVisible", 1))
    {  hideMouse = true;
    }

    
    int nTiles = map->get("NumTiles", 1);
    std::string tileStr = "Tile";
    for (int i=0;i<nTiles;i++) {
      if (nTiles > 1) {
        tileStr = "Tile" + intToString(i+1);
      }
        
      
      topLeft  = map->get(tileStr + "TopLeft", Vector3(-0.65, 0.5, 0.0));
      topRight = map->get(tileStr + "TopRight", Vector3( 0.65, 0.5, 0.0));
      botLeft  = map->get(tileStr + "BotLeft", Vector3(-0.65,-0.5, 0.0));
      botRight = map->get(tileStr + "BotRight", Vector3( 0.65,-0.5, 0.0));
      nearclip = map->get(tileStr + "NearClip", 0.01);
      farclip  = map->get(tileStr + "FarClip", 100.0);
      viewportX  = map->get(tileStr + "ViewportX", 0);
      viewportY  = map->get(tileStr + "ViewportY", 0);
      viewportW  = map->get(tileStr + "ViewportW", 0);
      viewportH  = map->get(tileStr + "ViewportH", 0);
      
      
      if (gwinsettings.stereo)
      { // If we're creating a stereo window, then set the display render type to stereo
        renderType = DisplayTile::TILE_STEREO;
      }
      else
      { // Else default to mono unless any of these other special
        // rendering types were specified as the TileRenderType.
        std::string rtStr = map->get(tileStr + "RenderType", "MONO");
        
        if (rtStr == "MONO_LEFT")
        {  renderType = DisplayTile::TILE_MONO_LEFT;
        }
        else if (rtStr == "MONO_RIGHT")
        {  renderType = DisplayTile::TILE_MONO_RIGHT;
        }
        else if (rtStr == "CHECKERBOARD_STEREO")
        {  renderType = DisplayTile::TILE_CHECKERBOARD_STEREO;
        }
        else if (rtStr == "BLUELINE_STEREO")
        { renderType = DisplayTile::TILE_BLUELINE_STEREO;
        }
        else if (rtStr == "HORIZONTAL_STEREO")
        {  renderType = DisplayTile::TILE_HORIZONTAL_STEREO;
        }
        else if (rtStr == "SIDE_BY_SIDE")
        {  renderType = DisplayTile::TILE_SIDE_BY_SIDE;
        }
      }
      
      DisplayTile tile = DisplayTile(topLeft, topRight, botLeft, botRight,
                                     renderType, nearclip, farclip,
                                     viewportX, viewportY, viewportW, viewportH);
      
      initialHeadFrame = map->get(tileStr + "InitialHeadFrame",
                                  CoordinateFrame( Vector3( 0, 0, 1 )));
      
      _cameras.append(new ProjectionVRCamera(tile, initialHeadFrame));
    }  // end for ntiles      
    
    // This will only work (and is only expected to be needed) for a single tile display
    if (map->get("TileAutoAdjustToResolution", iFalse) == 1) { 
      tileAutoAdjustToResolution = true;
    }

    // This will only work (and is only expected to be needed) for a single tile display
    if( map->get("TileOmnimap", iFalse)) {
      setupOmnimap = true;
      omnimapConfig = map->get("TileOmnimapConfig", "OmniMapConfig\\omnimap_startup.lua");
	  omnimapLuaRes = map->get("TileOmnimapLuaRes", "OmniMapConfig\\Resources\\");
    }

    _swapEyes = map->get("TileSwapEyes", iFalse);
  }
  else
  { // Can't find VR setup config file
    Array<std::string> configFiles;
    int df=0;

    while ((df < _dataFilePaths.size()) && (configFiles.size() == 0))
    { std::string pattern = replaceEnvVars(_dataFilePaths[df]) + "*.vrsetup";

      FileSystem::getFiles(pattern, configFiles, true);
      df++;
    }

    if (vrSetup == "")
    { cerr << "No argument specified for a VR setup to load." << endl;
    }
    else
    { cerr << "Could not find a valid VR setup named: " << vrSetup << endl;
    }

    cerr << "Continuting with a default desktop setup." << endl;
    cerr << "-----------------------------------------------------------------" << endl;
    cerr << "Installed VR setups are listed below:" << endl;

    if (configFiles.size() == 0)
    {  cerr << "None!" << endl;
    }
    else
    { Array<std::string> name, desc;

      for (int i=0;i<configFiles.size();i++)
      { std::string drive, base, ext;
        Array<std::string> path;

        parseFilename(configFiles[i], drive, path, base, ext);
        name.append(base);
        ConfigMapRef map = new ConfigMap(configFiles[i], _log);
        desc.append(map->get("Description", "Unknown description"));
      }

      unsigned int maxLen = 0;

      for (int i=0;i<name.size();i++)
      { if (name[i].size() > maxLen)
        {  maxLen = name[i].size();
        }
      }

      for (int i=0;i<name.size();i++)
      { cerr << name[i];
        int nspaces = maxLen - name[i].size() + 2;

        for (int s=0;s<nspaces;s++)
        {  cerr << " ";
        }

        cerr << desc[i] << endl;
      }

      cerr << "-----------------------------------------------------------------" << endl;
    }

    DisplayTile tile = DisplayTile::defaultDesktopTile();
    _cameras.append(new ProjectionVRCamera(tile, initialHeadFrame));

  }


  /* Note: there is a bug currently in some nvidia drivers - they
   can't open fullscreen stereo windows via whatever method SDLWindow
   and X11Window use internally, but GlutWindow works.  Ideally we
   would simply use Win32Window on Windows and X11Window on everything
   else, but until these implementations are ready for prime time use,
   we detect the buggy situation and use GlutWindow.  USE_GLUT needs
   to be defined in the Makefiles to compile and link in this Glut
   code.
   Brown Note: SDLWindow works on CS style linux machines, but not
   on CASCV's CentOS machines at the Cave.
  */
#ifdef USE_GLUT
    cout << "Starting a GlutWindow -- USE_GLUT was defined when compiling VRApp." << endl;
    _gwindow = new GlutWindow(gwinsettings);
#else
     VRApp::initCreateWindow(gwinsettings);
#endif

  cout << "New render device" << endl;
  _renderDevice = new RenderDevice();
  cout << "Render device init" << endl;
  _renderDevice->init(_gwindow);
  cout << "Render device reset state" << endl;
  _renderDevice->resetState();

  // Expand the tile in room coordinates to agree with the aspect
  // ratio of the display.  For a VR display, you should really
  // measure the exact dimensions of the physical display with a ruler
  // and avoid this auto adjustment, but for a simple desktop display,
  // this is a reasonable approximation to handle viewports of a
  // variety of sizes and aspect ratios.  Assumes square pixels.
  if (tileAutoAdjustToResolution)
  { Vector3 tileCtr   = (topLeft + topRight + botLeft + botRight) / 4.0;
    Vector3 tileUp    = (topLeft - botLeft).unit();
    Vector3 tileRight = (topRight - topLeft).unit();

    double  tileW     = (topRight - topLeft).magnitude();
    double  tileH     = (topRight - botRight).magnitude();
    double  ratio     = (double)_gwindow->width() / (double)_gwindow->height();

    if (ratio >= 1.0)
    { double newTileW = ratio * tileH;

      topLeft  = tileCtr + 0.5*tileH*tileUp - 0.5*newTileW*tileRight;
      topRight = tileCtr + 0.5*tileH*tileUp + 0.5*newTileW*tileRight;
      botLeft  = tileCtr - 0.5*tileH*tileUp - 0.5*newTileW*tileRight;
      botRight = tileCtr - 0.5*tileH*tileUp + 0.5*newTileW*tileRight;
    }
    else
    { double newTileH = tileW / ratio;

      topLeft  = tileCtr + 0.5*newTileH*tileUp - 0.5*tileW*tileRight;
      topRight = tileCtr + 0.5*newTileH*tileUp + 0.5*tileW*tileRight;
      botLeft  = tileCtr - 0.5*newTileH*tileUp - 0.5*tileW*tileRight;
      botRight = tileCtr - 0.5*newTileH*tileUp + 0.5*tileW*tileRight;
    }

    DisplayTile tile = DisplayTile(topLeft, topRight, botLeft, botRight,
        renderType, nearclip, farclip, 0, 0, _gwindow->width(), _gwindow->height());
    _cameras[_cameras.size()-1] = new ProjectionVRCamera(tile, initialHeadFrame);
  }


  if (hideMouse)
  { _gwindow->incMouseHideCount();
  }

  _widgetManager = WidgetManager::create(_gwindow);
  _userInput = new UserInput(_gwindow);

  if (_isAClusterClient)
  { clientSetup();
  }

#ifdef USE_OMNIMAP
    if(setupOmnimap) {
      _omnimapLib = new OmniMap(_renderDevice->width(),_renderDevice->height(), omnimapConfig.c_str(), omnimapLuaRes.c_str());
    }
#endif

  initVRAppMutex.unlock();
  return;
}



void  VRApp::init( GWindow         *gwindow,
                   RenderDevice    *renderDevice,
                   DisplayTile      tile,
             Array<InputDevice *>   inputDevices,
                   CoordinateFrame  initialHeadFrame,
             const std::string     &clusterServerName,
                   Log             *log )
{
  _gwindow = gwindow;
  _renderDevice = renderDevice;
  _inputDevices = inputDevices;
  _log = log;
  _isAClusterClient = (clusterServerName != "");
  _clusterServerName = clusterServerName;

  if (_log == NULL)
  {  _log = new Log("log.txt");
  }

  _cameras.append(new ProjectionVRCamera(tile, initialHeadFrame));

  if (_isAClusterClient)
  {  clientSetup();
  }
}


void  VRApp::doUserInput(Array<EventRef> &events)
{
  // Respond to events generated by input devices since the last frame was rendered

  /** Typical setup:
  for (int i=0;i<events.size();i++)
  {
    if (events[i]->getName() == "Wand_Tracker")
    {
      CoordinateFrame newFrame = events[i]->getCoordinateFrameData();
    }
    else if (events[i]->getName() == "kbd_SPACE_down")
    {
      cout << "Pressed the space key." << endl;
    }
    else if (events[i]->getName() == "Mouse_Pointer")
    {
      cout << "New mouse position = " << events[i]->get2DData() << endl;
    }
  }
  **/
}


void  VRApp::guiProcessGEvents( Array<GEvent>           &gevents,
                                Array<VRG3D::EventRef>  &newGuiEvents )
{
  Array<GEvent> returnEvents;
  _userInput->beginEvents();

  for (int i=0;i<gevents.size();i++)
  {
    if (gevents[i].type == GEventType::GUI_ACTION)
    { Array<GuiButton*> keys = _guiBtnToEventMap.getKeys();

      for (int k=0;k<keys.size();k++)
      { if (gevents[i].gui.control == keys[k])
        {  newGuiEvents.append(new VRG3D::Event(_guiBtnToEventMap[keys[k]]));
        }
      }
    }

    if (!WidgetManager::onEvent(gevents[i], _widgetManager))
    { returnEvents.append(gevents[i]);
    }

    // BUG: There is a bug in G3D::GuiButton that makes it consume all
    // mouse button up events once it has been pressed.  The "fix"
    // here is to always report mouse up events, even if they should
    // be consumed by a G3D Gui element.  This isn't a great fix
    // though, it could screw up some programs.
    //
    else if (gevents[i].type == GEventType::MOUSE_BUTTON_UP)
    {  returnEvents.append(gevents[i]);
    }

    // BUG: G3D's UserInput class crashes with an array out of bounds
    // error when the option key on a mac is pressed because it's
    // key value is < 0, so avoid processing keys with negative keycodes.
    //
    if (((gevents[i].type == GEventType::KEY_UP)     ||
         (gevents[i].type == GEventType::KEY_DOWN))  &&
         (gevents[i].key.keysym.sym < 0))
    {
      // do nothing
    }
    else
    {  _userInput->processEvent(gevents[i]);
    }
  }

  _userInput->endEvents();
  _widgetManager->onUserInput(_userInput);

  gevents = returnEvents;
}


void  VRApp::doGraphics(RenderDevice *rd)
{
   // Example of drawing a simple piece of geometry using G3D::Draw
   //
   Draw::axes( CoordinateFrame(), rd,
               Color3::red(), Color3::green(), Color3::blue(), 0.25 );
}

  void  VRApp::doGraphics(RenderDevice *rd, bool leftEye)
{

   doGraphics(rd);
   
}


void  VRApp::run()
{
  if (_gwindow->requiresMainLoop())
  { _gwindow->pushLoopBody(gWindowLoopCallback, this);
    _gwindow->runMainLoop();
  }
  else
  { while (!_endProgram)
    { oneFrame();
    }
  }
}

#ifdef USE_OMNIMAP

// Nest render() function - or display() function – inside the fun() function.
// The fun() funcion will be called once for each OmniMap render channel.
// The channel being rendered is passed as an argument. 
void omnimapdrawfunction(OmniMapChannelBase *chan, void *vrapp)
{
	// Shows how to get a channel name from the channel pointer.
	const char * channelName = _omnimapLib->GetChannelName(chan);
	// beginRenderToChannel sets up the viewing offset and projection transforms in the projection matrix, 
	// and directs rendering to the off screen render buffer.  
	// By default the off screen render buffer is implemented in a frame buffer object.  
	// It is very important that the viewing offset and projection matrix defined by the
	// channel are used for rendering the channel.  If not, the channels will not line up
	// when rendered to the final screen.  For access to these transforms see the class documentation on
	// the OmniMap_Channel and OmniMapChannelBase classes.
	// PBuffers and back buffer rendering are also supported.  The modele
	chan->beginRenderToChannel();
	// Shows how to get metadata set in the lua scripts from a channel
	// In this case the metadata is called ExampleMetaData and is an integer.
	GenericDataContainer * cont= chan->ChannelMetaData.IndexDataMap("ExampleMetaData");
	int ExampleMetaData= 0; if(cont)  ExampleMetaData = cont->GetINT();
	// Shows how to get a number variable from the lua script
	double var2fromlua = _omnimapLib->ScriptingEngine->GetVariableNumber("GLOBAL_clipNear");
	// Shows how to get a number variable from the lua script
	double var4fromlua = _omnimapLib->ScriptingEngine->GetVariableNumber("GLOBAL_clipFar");

	// Render the scene.
	((VRApp*)vrapp)->oneFrameGraphics();

	// Pop the transforms from the matrix stack, and reset stop rendering to the off
	// screen buffer.
	chan->endRenderToChannel();
}
#endif



void  VRApp::oneFrame()
{
  _frameCounter++;

  // Clients should request new events from the server
  if (_isAClusterClient)
  { clientRequestEvents();
    vrg3dSleepSecs(CLIENT_SLEEP);
  }

  // Collect new events from input devices
  Array<EventRef> events;

  for (int i=0;i<_inputDevices.size();i++)
  { _inputDevices[i]->pollForInput(events);
  }

  // Poll graphics window for mouse and kbd events
  //
  Array<GEvent> gEvents;
  pollWindowForGEvents(_renderDevice, gEvents);

  // Respond to any G3D::Gui* elements registered with our
  // _widgetManager and convert registered GuiButton events to
  // VRG3D::Events.
  //
  guiProcessGEvents(gEvents, events);

  // Convert remaining unprocessed GEvents to VRG3D::Events.
  //
  appendGEventsToEvents( _renderDevice, _cameras[0]->tile,
                         events,        gEvents,
                         _curMousePos,  _adjustTileOnResizeEvent );


  // Clients should check for new events from the server
  //
  if (_isAClusterClient)
  {  clientReceiveEvents(events);
  }

  // Respond here to VRG3D system-level events (head tracking, update synced clock)
  //
  for (int i=0;i<events.size();i++)
  { if (events[i]->getName() == "Head_Tracker")
    { //cout << frameCounter << " " << events[i]->getCoordinateFrameData().translation << endl;
      for (int c=0;c<_cameras.size();c++) {
        _cameras[c]->updateHeadFrame(events[i]->getCoordinateFrameData());
      }
    }
    else if (events[i]->getName() == "SynchedTime")
    { if (SynchedSystem::getTimeUpdateMethod() == SynchedSystem::USE_LOCAL_SYSTEM_TIME)
      {
        // We must be running in a cluster, and this is the very first
        // SynchedTime event so use it as the program start time.
        SynchedSystem::setProgramUpdatesTime(events[i]->get1DData());
      }

      SynchedSystem::updateLocalTime(events[i]->get1DData());
    }
    else if (events[i]->getName() == "Shutdown")
    { _endProgram = true;
    }
  }

  // Subclasses should respond to events that interest them by filling in this method
  doUserInput(events);

  // Render Graphics

  _renderDevice->setColorClearValue(_clearColor);

  if (_frameCounter == 1)
  {  _renderDevice->beginFrame();
  }


#ifdef USE_OMNIMAP
  if(_omnimapLib != NULL) {
	_omnimapLib->ForEachChannel(omnimapdrawfunction, this);
	_omnimapLib->PostRender();
  }
  else {
	oneFrameGraphics();
  }
#else 
  // Subclasses should reimplement the doGraphics routine to draw
  // their own graphics, but should NOT call RenderDevice::clear(),
  // beginFrame(), or endFrame().  In cases where additional control
  // of the gfx loop is needed, such as the ability to apply a custom
  // camera/projection matrix, the oneFrameGraphics() routine may be
  // reimplemented.
  oneFrameGraphics();
#endif

  // Clients should tell the server they are ready and wait for an an ok to
  // swap signal.
  //
  if (_isAClusterClient)
  { glFlush();
    clientRequestAndWaitForOkToSwap();
  }

  _renderDevice->endFrame();
  // In G3D ver 6.08+ swap buffers is actually called in beginFrame(), so we want
  // this very close to the cluster swaplock.
  _renderDevice->beginFrame();
}




// The graphics rendering portion of the loop setup inside the oneFrame method.
//
void  VRApp::oneFrameGraphics()
{    
  // set whether the main eye is a left eye or not
  bool leftEye = _swapEyes ? false : true;

  if (_cameras[0]->tile.renderType == DisplayTile::TILE_STEREO) { 
    _renderDevice->setDrawBuffer(RenderDevice::DRAW_BACK_LEFT);
    glDrawBuffer(GL_BACK_LEFT);
    _renderDevice->clear(true,true,true);
    for (_activeCameraNum=0;_activeCameraNum<_cameras.size();_activeCameraNum++) {
      if (_cameras.size() > 1) {
        glViewport(_cameras[_activeCameraNum]->tile.viewportX, _cameras[_activeCameraNum]->tile.viewportY,
                   _cameras[_activeCameraNum]->tile.viewportW, _cameras[_activeCameraNum]->tile.viewportH);
      }
      if(_swapEyes) {
	_cameras[_activeCameraNum]->applyProjection(_renderDevice, ProjectionVRCamera::RightEye);
      }
      else {
	_cameras[_activeCameraNum]->applyProjection(_renderDevice, ProjectionVRCamera::LeftEye);
      }
      doGraphics(_renderDevice, leftEye);
    }
    _renderDevice->setDrawBuffer(RenderDevice::DRAW_BACK_RIGHT);
    glDrawBuffer(GL_BACK_RIGHT);
    _renderDevice->clear(true,true,true);
    for (_activeCameraNum=0;_activeCameraNum<_cameras.size();_activeCameraNum++) {
      if (_cameras.size() > 1) {
        glViewport(_cameras[_activeCameraNum]->tile.viewportX, _cameras[_activeCameraNum]->tile.viewportY,
				   _cameras[_activeCameraNum]->tile.viewportW, _cameras[_activeCameraNum]->tile.viewportH);
      }
      if(_swapEyes) {
	_cameras[_activeCameraNum]->applyProjection(_renderDevice, ProjectionVRCamera::LeftEye);
      }
      else {
	_cameras[_activeCameraNum]->applyProjection(_renderDevice, ProjectionVRCamera::RightEye);
      }
      doGraphics(_renderDevice, !leftEye);  
    }
  }
  else if (_cameras[0]->tile.renderType == DisplayTile::TILE_CHECKERBOARD_STEREO)
  { _renderDevice->setDrawBuffer(RenderDevice::DRAW_BACK);
    glDrawBuffer(GL_BACK);
    _renderDevice->clear(true,true,true);
    
    // Draw checkerboard into stencil buffer
    _renderDevice->pushState();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    checkerboard_stencil(_renderDevice->width(), _renderDevice->height());
    glPopAttrib();
    _renderDevice->popState();
    
    _renderDevice->clear(true,true,false);
    
    glEnable(GL_STENCIL_TEST);
    glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_NOTEQUAL, 1, 1);
    
    if (glIsEnabled(GL_STENCIL_TEST_TWO_SIDE_EXT))
    { glActiveStencilFaceEXT(GL_BACK);
      glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
      glStencilFunc(GL_NOTEQUAL, 1, 1);
      glActiveStencilFaceEXT(GL_FRONT);
    }
    
    _cameras[0]->applyProjection(_renderDevice, ProjectionVRCamera::LeftEye);
    doGraphics(_renderDevice, leftEye);
    
    glStencilFunc(GL_EQUAL, 1, 1);
    
    if (glIsEnabled(GL_STENCIL_TEST_TWO_SIDE_EXT))
    { glActiveStencilFaceEXT(GL_BACK);
      glStencilFunc(GL_EQUAL, 1, 1);
      glActiveStencilFaceEXT(GL_FRONT);
    }
    
    _cameras[0]->applyProjection(_renderDevice, ProjectionVRCamera::RightEye);
    doGraphics(_renderDevice, !leftEye);
    
    glDisable(GL_STENCIL_TEST);
  }
  else if (_cameras[0]->tile.renderType == DisplayTile::TILE_HORIZONTAL_STEREO)
  { _renderDevice->setDrawBuffer(RenderDevice::DRAW_BACK);
    glDrawBuffer(GL_BACK);
    _renderDevice->clear(true,true,true);
    
    // Draw checkerboard into stencil buffer
    _renderDevice->pushState();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    horizontal_stencil(_renderDevice->width(), _renderDevice->height());
    glPopAttrib();
    _renderDevice->popState();
    
    _renderDevice->clear(true,true,false);
    
    glEnable(GL_STENCIL_TEST);
    glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_NOTEQUAL, 1, 1);
    
    if (glIsEnabled(GL_STENCIL_TEST_TWO_SIDE_EXT))
    { glActiveStencilFaceEXT(GL_BACK);
      glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
      glStencilFunc(GL_NOTEQUAL, 1, 1);
      glActiveStencilFaceEXT(GL_FRONT);
    }
    
    _cameras[0]->applyProjection(_renderDevice, ProjectionVRCamera::LeftEye);
    doGraphics(_renderDevice, leftEye);
    
    glStencilFunc(GL_EQUAL, 1, 1);
    
    if (glIsEnabled(GL_STENCIL_TEST_TWO_SIDE_EXT))
    { glActiveStencilFaceEXT(GL_BACK);
      glStencilFunc(GL_EQUAL, 1, 1);
      glActiveStencilFaceEXT(GL_FRONT);
    }
    
    _cameras[0]->applyProjection(_renderDevice, ProjectionVRCamera::RightEye);
    doGraphics(_renderDevice, !leftEye);
    
    glDisable(GL_STENCIL_TEST);
  }
  
  //////////////////
  else if (_cameras[0]->tile.renderType == DisplayTile::TILE_SIDE_BY_SIDE)
  { 
    _renderDevice->setDrawBuffer(RenderDevice::DRAW_BACK);
    
    glDrawBuffer(GL_BACK);
    _renderDevice->clear(true,true,true);
    
    _renderDevice->setViewport(Rect2D(Vector2(_gwindow->width()/2, _gwindow->height())) + Vector2(_gwindow->width()/2,0));
    
    _cameras[0]->applyProjection(_renderDevice, ProjectionVRCamera::LeftEye);
    doGraphics(_renderDevice, leftEye);
    
    _renderDevice->setViewport(Rect2D(Vector2(_gwindow->width()/2, _gwindow->height())));
    
    _cameras[0]->applyProjection(_renderDevice, ProjectionVRCamera::RightEye);
    doGraphics(_renderDevice, !leftEye);
    
  }
  ///////////////////
  
  else if (_cameras[0]->tile.renderType == DisplayTile::TILE_BLUELINE_STEREO)
  { // Alternately render left then right frames using the length of a blue line in the last row of pixels as
    // a code for whether the frame is a left eye frame or right eye frame.
    _renderDevice->setDrawBuffer(RenderDevice::DRAW_BACK);
    glDrawBuffer(GL_BACK);
    _renderDevice->clear(true,true,true);
    
    if (_blueLineStereoLeftFrame)
    { _cameras[0]->applyProjection(_renderDevice, ProjectionVRCamera::LeftEye);
    }
    else
    { _cameras[0]->applyProjection(_renderDevice, ProjectionVRCamera::RightEye);
    }
    
    doGraphics(_renderDevice, leftEye);
    
    // Draw blue line on bottom row of pixels to signal left or right eye frame
    _renderDevice->push2D();
    int wDiv4 = _renderDevice->width()/4;
    int h = _renderDevice->height() - 1;
    
    glColor3d(0.0,0.0,0.0);
    glBegin(GL_LINES);
    glVertex2i(0,h);
    glVertex2i(_renderDevice->width(),h);
    glEnd();
    
    // Draw a blue line of the correct length, about 40% from the left
    glColor3d(0.0,0.0,1.0);
    //glColor3d(1.0,1.0,1.0);
    glBegin(GL_LINES);
    glVertex2i(0, h);
    
    if (_blueLineStereoLeftFrame)
    {  glVertex2i(wDiv4, h);
    }
    else
    {  glVertex2i(3*wDiv4, h);
    }
    
    glEnd();
    _renderDevice->pop2D();
    
    _blueLineStereoLeftFrame = !_blueLineStereoLeftFrame;
  }
  else if (_cameras[0]->tile.renderType == DisplayTile::TILE_MONO_LEFT)
  { _renderDevice->setDrawBuffer(RenderDevice::DRAW_BACK_LEFT);
    glDrawBuffer(GL_BACK);
    _renderDevice->clear(true,true,true);
    _cameras[0]->applyProjection(_renderDevice, ProjectionVRCamera::LeftEye);
    doGraphics(_renderDevice, leftEye);
  }
  else if (_cameras[0]->tile.renderType == DisplayTile::TILE_MONO_RIGHT)
  { _renderDevice->setDrawBuffer(RenderDevice::DRAW_BACK_LEFT);
    glDrawBuffer(GL_BACK);
    _renderDevice->clear(true,true,true);
    _cameras[0]->applyProjection(_renderDevice, ProjectionVRCamera::RightEye);
    doGraphics(_renderDevice, !leftEye);
  }
  else
  { _renderDevice->setDrawBuffer(RenderDevice::DRAW_BACK_LEFT);
    glDrawBuffer(GL_BACK);
    _renderDevice->clear(true,true,true);
    for (_activeCameraNum=0;_activeCameraNum<_cameras.size();_activeCameraNum++) {
      if (_cameras.size() > 1) {
        glViewport(_cameras[_activeCameraNum]->tile.viewportX, _cameras[_activeCameraNum]->tile.viewportY,
                   _cameras[_activeCameraNum]->tile.viewportW, _cameras[_activeCameraNum]->tile.viewportH);
      }
      _cameras[_activeCameraNum]->applyProjection(_renderDevice, ProjectionVRCamera::Cyclops);
      doGraphics(_renderDevice, leftEye);
    }
  }
  

  return;
}


std::string  VRApp::findVRG3DDataFile(const std::string &filename)
{
  if (( filename.compare( 0, 2, "//" ) == 0 )    ||
      ( filename.compare( 0, 2, "\\\\" ) == 0 ))
  {
     cerr << "The filename path is a network share and is unsupported." << endl;
     cerr << "    filename = " << filename << endl;
  }
  else if (( FileSystem::currentDirectory().compare( 0, 2, "//" ) == 0)     ||
           ( FileSystem::currentDirectory().compare( 0, 2, "\\\\" ) == 0 ))
  {
     cerr << "The current directory is a network share and is unsupported." << endl;
     cerr << "    current directory = " << FileSystem::currentDirectory() << endl;
  }
  else
  {  for (int i = 0; i < _dataFilePaths.size(); i++)
     { std::string fname = _dataFilePaths[i] + filename;

       if (FileSystem::exists(fname))
       {  return fname;
       }
     }

     cerr << "Could not find data file as either:" << endl;

     for (int i = 0; i < _dataFilePaths.size(); i++)
     {  std::string fname = _dataFilePaths[i] + filename;
        cerr << i << ". " << fname << endl;
     }
  }

  return "";
}




// ----------------------------------------------------------------------------


void  VRApp::setupInputDevicesFromConfigFile( const std::string  &filename,
                                                         Log     *log,
                                            Array<InputDevice*>  &devices )
{
  initVRAppMutex.lock();
  ConfigMapRef map = new ConfigMap( filename, log );

  Array<std::string> devnames = splitStringIntoArray( map->get( "InputDevices",
                                                                "" ));

  for (int i = 0; i < devnames.size(); i++ )
  { std::string devname = devnames[i];

    devices.append( InputDevice::newInputDevice( devname, log, map ));
  }

  initVRAppMutex.unlock();

  return;
}



void  VRApp::pollWindowForGEvents(RenderDevice *rd, Array<GEvent> &gEvents)
{
  GWindow *gwindow = rd->window();

  if (!gwindow)
  {  return;
  }

  GEvent event;

  while (gwindow->pollEvent(event))
  {  gEvents.append(event);
  }
}


/// Converts GEvents to VRG3D::Events and appends them to the events array
void  VRApp::appendGEventsToEvents( RenderDevice     *rd,
                                    DisplayTile      &tile,
                                    Array<EventRef>  &events,
                                    Array<GEvent>    &gEvents,
                                    Vector2          &mousePos,
                                    bool              adjustTileOnResizeEvent )
{
  bool gotMouseMotion = false;

  for (int i = 0; i < gEvents.size(); i++)
  {
    GEvent event = gEvents[i];
    switch(event.type)
    {
    case GEventType::QUIT:
      exit(0);
      break;

    case GEventType::VIDEO_RESIZE:
      {
        events.append(new Event("WindowResize", Vector2(event.resize.w, event.resize.h)));

        double oldw = rd->width();
        double oldh = rd->height();

        ///rd->notifyResize(event.resize.w, event.resize.h);
        //rd->resize(event.resize.w, event.resize.h);
        Rect2D full = Rect2D::xywh(0, 0, rd->width(), rd->height());
        rd->setViewport(full);


        if (adjustTileOnResizeEvent)
        {
          double w = rd->width();
          double h = rd->height();

          /** recompute room coordinates of the tile display
              if width changes more than height, then width controls what appears
              to be zoom and height is adjusted so that what was in the center
              of the screen before remains in the center of the screen.  if
              height changes more, then we do the opposite.
          */

          // scale factors
          double scalew = w / oldw;
          double scaleh = h / oldh;

          if (fabs(scalew-1.0) >= fabs(scaleh-1.0))
          {
            double newh = scalew*oldh;
            double frach = h / newh;
            double newy = tile.topLeft[1] + frach*(tile.botLeft[1] - tile.topLeft[1]);
            double diff_y_div2 = (newy-tile.botLeft[1]) / 2.0;
            tile.topLeft[1] -= diff_y_div2;
            tile.topRight[1] -= diff_y_div2;
            tile.botLeft[1] = newy - diff_y_div2;
            tile.botRight[1] = newy - diff_y_div2;
          }
          else
          {
            double neww = scaleh*oldw;
            double fracw = w / neww;
            double newx = tile.topLeft[0] + fracw*(tile.topRight[0] - tile.topLeft[0]);
            double diff_x_div2 = (newx-tile.topRight[0]) / 2.0;
            tile.topLeft[0] -= diff_x_div2;
            tile.topRight[0] = newx - diff_x_div2;
            tile.botLeft[0] -= diff_x_div2;
            tile.botRight[0] = newx - diff_x_div2;
          }

          // recompute room2tile matrix
          Vector3 center = (tile.topLeft + tile.topRight + tile.botLeft + tile.botRight) / 4.0;
          Vector3 x = (tile.topRight - tile.topLeft).unit();
          Vector3 y = (tile.topLeft - tile.botLeft).unit();
          Vector3 z = x.cross(y).unit();
          Matrix3 rot(x[0],y[0],z[0],x[1],y[1],z[1],x[2],y[2],z[2]);
          CoordinateFrame tile2room(rot,center);
          tile.room2tile  = tile2room.inverse();
        }
      }

      break;

////////////////////////////////////////+++
    case GEventType::GUI_ACTION:
      {
        std::string controlName = event.gui.control->caption();
        
        events.append(new Event("gui_Action", controlName));
      }
      break;
////////////////////////////////////////+++


    case GEventType::KEY_DOWN:
      {
        std::string keyname = getKeyName(event.key.keysym.sym, event.key.keysym.mod);
        events.append(new Event("kbd_" + keyname + "_down"));
      }

      break;

    case GEventType::KEY_UP:
      {
        std::string keyname = getKeyName(event.key.keysym.sym, event.key.keysym.mod);
        events.append(new Event("kbd_" + keyname + "_up"));
      }

      break;

    case GEventType::MOUSE_MOTION:
      {
      // Mouse Coordiante System Convention
       /**                (1,1)
         +-------------+
         |             |
         |    (0,0)    |
         |             |
         +-------------+
      (-1,-1)               **/

      gotMouseMotion = true;
      double fracx = (double)event.motion.x / (double)rd->width();
      double fracy = (double)event.motion.y / (double)rd->height();
      Vector2 vmouse((fracx * 2.0) - 1.0, -((fracy * 2.0) - 1.0));
      events.append(new Event("Mouse_Pointer",vmouse));
      mousePos = vmouse;
      }
      break;

    case GEventType::MOUSE_BUTTON_DOWN:
      {
      switch (event.button.button)
      {
      case 0: //SDL_BUTTON_LEFT:
        events.append(new Event("Mouse_Left_Btn_down", mousePos));
        break;
      case 1: //SDL_BUTTON_MIDDLE:
        events.append(new Event("Mouse_Middle_Btn_down", mousePos));
        break;
      case 2: //SDL_BUTTON_RIGHT:
        events.append(new Event("Mouse_Right_Btn_down", mousePos));
        break;
      case 3:
        events.append(new Event("Mouse_WheelUp_Btn_down", mousePos));
        break;
      case 4:
        events.append(new Event("Mouse_WheelDown_Btn_down", mousePos));
        break;
      default:
        events.append(new Event("Mouse_" + intToString(event.button.button) + "_Btn_down", mousePos));
        break;
      }
      }
      break;

    case GEventType::MOUSE_BUTTON_UP:
      {
      switch (event.button.button)
      {
      case 0: //SDL_BUTTON_LEFT:
        events.append(new Event("Mouse_Left_Btn_up", mousePos));
        break;
      case 1: //SDL_BUTTON_MIDDLE:
        events.append(new Event("Mouse_Middle_Btn_up", mousePos));
        break;
      case 2: //SDL_BUTTON_RIGHT:
        events.append(new Event("Mouse_Right_Btn_up", mousePos));
        break;
      case 3:
        events.append(new Event("Mouse_WheelUp_Btn_up", mousePos));
        break;
      case 4:
        events.append(new Event("Mouse_WheelDown_Btn_up", mousePos));
        break;
      default:
        events.append(new Event("Mouse_" + intToString(event.button.button) + "_Btn_up", mousePos));
        break;
      }
      }
      break;
    }
  }

  if (!gotMouseMotion) {
    // Events may not be coming through as events for this window
    // type, try polling
    int x,y;
    G3D::uint8 btns;
    rd->window()->getRelativeMouseState(x, y, btns);
    if (Vector2(x,y) != mousePos) {
      double fracx = (double)x / (double)rd->width();
      double fracy = (double)y / (double)rd->height();
      Vector2 vmouse((fracx * 2.0) - 1.0, -((fracy * 2.0) - 1.0));
      events.append(new Event("Mouse_Pointer",vmouse));
      mousePos = vmouse;
    }
  }

}


std::string
VRApp::getKeyName(GKey::Value key, GKeyMod mod)
{
  std::string name;

  switch (key) {
  case GKey::F1:
    name =  std::string("F1");
    break;
  case GKey::F2:
    name =  std::string("F2");
    break;
  case GKey::F3:
    name =  std::string("F3");
    break;
  case GKey::F4:
    name =  std::string("F4");
    break;
  case GKey::F5:
    name =  std::string("F5");
    break;
  case GKey::F6:
    name =  std::string("F6");
    break;
  case GKey::F7:
    name =  std::string("F7");
    break;
  case GKey::F8:
    name =  std::string("F8");
    break;
  case GKey::F9:
    name =  std::string("F9");
    break;
  case GKey::F10:
    name =  std::string("F10");
    break;
  case GKey::F11:
    name =  std::string("F11");
    break;
  case GKey::F12:
    name =  std::string("F12");
    break;
  case GKey::KP0:
    name =  std::string("KP0");
    break;
  case GKey::KP1:
    name =  std::string("KP1");
    break;
  case GKey::KP2:
    name =  std::string("KP2");
    break;
  case GKey::KP3:
    name =  std::string("KP3");
    break;
  case GKey::KP4:
    name =  std::string("KP4");
    break;
  case GKey::KP5:
    name =  std::string("KP5");
    break;
  case GKey::KP6:
    name =  std::string("KP6");
    break;
  case GKey::KP7:
    name =  std::string("KP7");
    break;
  case GKey::KP8:
    name =  std::string("KP8");
    break;
  case GKey::KP9:
    name =  std::string("KP9");
    break;
  case GKey::KP_PERIOD:
    name =  std::string("KP_PERIOD");
    break;
  case GKey::KP_MULTIPLY:
    name =  std::string("KP_MULTIPLY");
    break;
  case GKey::KP_MINUS:
    name =  std::string("KP_MINUS");
    break;
  case GKey::KP_PLUS:
    name =  std::string("KP_PLUS");
    break;
  case GKey::KP_ENTER:
    name =  std::string("KP_ENTER");
    break;
  case GKey::KP_EQUALS:
    name =  std::string("KP_EQUALS");
    break;
  case GKey::LEFT:
    name =  std::string("LEFT");
    break;
  case GKey::RIGHT:
    name =  std::string("RIGHT");
    break;
  case GKey::UP:
    name =  std::string("UP");
    break;
  case GKey::DOWN:
    name =  std::string("DOWN");
    break;
  case GKey::PAGEUP:
    name =  std::string("PAGEUP");
    break;
  case GKey::PAGEDOWN:
    name =  std::string("PAGEDOWN");
    break;
  case GKey::HOME:
    name =  std::string("HOME");
    break;
  case GKey::END:
    name =  std::string("END");
    break;
  case GKey::INSERT:
    name =  std::string("INSERT");
    break;
  case GKey::BACKSPACE:
    name =  std::string("BACKSPACE");
    break;
  case GKey::TAB:
    name =  std::string("TAB");
    break;
  case GKey::RETURN:
    name =  std::string("ENTER");
    break;
  case GKey::ESCAPE:
    name =  std::string("ESC");
    break;
  case GKey::SPACE:
    name =  std::string("SPACE");
    break;
  case 48:
    name =  std::string("0");
    break;
  case 49:
    name =  std::string("1");
    break;
  case 50:
    name =  std::string("2");
    break;
  case 51:
    name =  std::string("3");
    break;
  case 52:
    name =  std::string("4");
    break;
  case 53:
    name =  std::string("5");
    break;
  case 54:
    name =  std::string("6");
    break;
  case 55:
    name =  std::string("7");
    break;
  case 56:
    name =  std::string("8");
    break;
  case 57:
    name =  std::string("9");
    break;
  case 97:
    name =  std::string("A");
    break;
  case 98:
    name =  std::string("B");
    break;
  case 99:
    name =  std::string("C");
    break;
  case 100:
    name =  std::string("D");
    break;
  case 101:
    name =  std::string("E");
    break;
  case 102:
    name =  std::string("F");
    break;
  case 103:
    name =  std::string("G");
    break;
  case 104:
    name =  std::string("H");
    break;
  case 105:
    name =  std::string("I");
    break;
  case 106:
    name =  std::string("J");
    break;
  case 107:
    name =  std::string("K");
    break;
  case 108:
    name =  std::string("L");
    break;
  case 109:
    name =  std::string("M");
    break;
  case 110:
    name =  std::string("N");
    break;
  case 111:
    name =  std::string("O");
    break;
  case 112:
    name =  std::string("P");
    break;
  case 113:
    name =  std::string("Q");
    break;
  case 114:
    name =  std::string("R");
    break;
  case 115:
    name =  std::string("S");
    break;
  case 116:
    name =  std::string("T");
    break;
  case 117:
    name =  std::string("U");
    break;
  case 118:
    name =  std::string("V");
    break;
  case 119:
    name =  std::string("W");
    break;
  case 120:
    name =  std::string("X");
    break;
  case 121:
    name =  std::string("Y");
    break;
  case 122:
    name =  std::string("Z");
    break;
  case GKey::RSHIFT:
  case GKey::LSHIFT:
    name =  std::string("SHIFT");
    return name;
    break;
  case GKey::RCTRL:
  case GKey::LCTRL:
    name =  std::string("CTRL");
    return name;
    break;
  case GKey::RALT:
  case GKey::LALT:
    name =  std::string("ALT");
    return name;
    break;
  case GKey::PERIOD:
    name = ".";
    break;
  case GKey::COMMA:
    name = ",";
    break;
  case GKey::LEFTBRACKET:
    name = "[";
    break;
  case GKey::RIGHTBRACKET:
    name = "]";
    break;
  default:
    cerr << "Unknown keypress: " << (int)key << endl;
    name =  std::string("UNKNOWN");
    break;
  }

  if (mod & GKEYMOD_SHIFT)
    name = "SHIFT_" + name;
  if (mod & GKEYMOD_ALT)
    name = "ALT_" + name;
  if (mod & GKEYMOD_CTRL)
    name = "CTRL_" + name;
  if (mod & GKEYMOD_META)
    name = "META_" + name;

  return name;
}


GuiButton*
VRApp::addGuiButtonAndTrapEvent(GuiPane *pane,
                                const std::string &eventToGenerate,
                                const GuiText  &text,
                                GuiTheme::ButtonStyle style)
{
  GuiButton *b = pane->addButton(text, style);
  _guiBtnToEventMap.set(b,eventToGenerate);
  return b;
}



void  vrg3dSleepSecs(double secs)
{
  vrg3dSleepMsecs(secs*1000.0);
}


/******************************* not used
std::string  VRApp::findVRG3DDataFile(const std::string &filename)
{
  if (FileSystem::exists(filename)) {
    return filename;
  }
  else {
    std::string filename2 = replaceEnvVars("$(G)/lib/VRG3D/" + filename);

    if (FileSystem::exists(filename2)) {
      return filename2;
    }
    else {
      std::string filename3 = replaceEnvVars("$(G)/lib/VRG3D/vrsetup/" + filename);

      if (FileSystem::exists(filename3)) {
        return filename3;
      }
      else {
        cerr << "Could not find data file as either:" << endl;
        cerr << "1. " << filename << endl;
        cerr << "2. " << filename2 << endl; 
        cerr << "3. " << filename3 << endl; 
      }
    }
  }
  return "";
}
************************************/



} // end namespace
