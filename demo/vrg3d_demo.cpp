//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

/*
 * \author Daniel Keefe (dfk)
 *
 * \file  vrg3d-demo.cpp
 *
 */


#include <vrg3d/VRG3D.h>


/** This is a sample VR application using the VRG3D library.  Two key
    methods are filled in here: doGraphics() and doUserInput().  The
    code in these methods demonstrates how to draw graphics and
    respond to input from the mouse, the keyboard, 6D trackers, and VR
    wand buttons.
*/

using namespace G3D;

class MyVRApp : public VRApp
{
public:
  MyVRApp(const std::string &mySetup) : VRApp()
  {
    // initialize the VRApp
     Log  *demoLog = new Log("demo-log.txt");
     init(mySetup, demoLog);

     _mouseToTracker = new MouseToTracker(getCamera(), 2);

    _virtualToRoomSpace = CoordinateFrame();

    _clearColor = Color3(0.75, 0.75, 0.75);
  }

  virtual ~MyVRApp() {}

  void doUserInput(Array<VRG3D::EventRef> &events)
  {
    // MouseToTracker is a really helpful class for testing out VR
    // interactions from the desktop.  This call makes it respond to
    // mouse events and generate new events as if it were a 6DOF
    // tracking device.  We add the new events to the event queue and
    // process them as usual.

    Array<VRG3D::EventRef> newEvents;
    _mouseToTracker->doUserInput(events, newEvents);
    events.append(newEvents);


    for (int i=0;i<events.size();i++)
    {
      if (events[i]->getName() == "kbd_ESC_down")
      {
        while(glGetError() != GL_NO_ERROR)
        {  std::cout<<"Flushing gl errors"<<std::endl;
        }

        exit(0);
      }

      // Save all the tracker events that come in so we can use them in the doGraphics routine
      else if (endsWith(events[i]->getName(), "_Tracker"))
      {
        if (_trackerFrames.containsKey(events[i]->getName()))
        {
           _trackerFrames[events[i]->getName()] = events[i]->getCoordinateFrameData();
        }
        else
        {  _trackerFrames.set( events[i]->getName(),
                               events[i]->getCoordinateFrameData() );
        }
      }

      // Respond to events to do some simple navigation
      else if (events[i]->getName() == "kbd_LEFT_down")
      {  _virtualToRoomSpace = CoordinateFrame(Matrix3::fromAxisAngle(Vector3(0,1,0), toRadians(5.0))) * _virtualToRoomSpace;
      }
      else if (events[i]->getName() == "kbd_RIGHT_down")
      {  _virtualToRoomSpace = CoordinateFrame(Matrix3::fromAxisAngle(Vector3(0,1,0), toRadians(-5.0))) * _virtualToRoomSpace;
      }
      else if (events[i]->getName() == "kbd_UP_down")
      {  _virtualToRoomSpace = CoordinateFrame(Matrix3::fromAxisAngle(Vector3(1,0,0), toRadians(5.0))) * _virtualToRoomSpace;
      }
      else if (events[i]->getName() == "kbd_DOWN_down")
      {  _virtualToRoomSpace = CoordinateFrame(Matrix3::fromAxisAngle(Vector3(1,0,0), toRadians(-5.0))) * _virtualToRoomSpace;
      }
      else if (events[i]->getName() == "kbd_SHIFT_LEFT_down")
      {  _virtualToRoomSpace = CoordinateFrame(Vector3(-0.1,0,0)) * _virtualToRoomSpace;
      }
      else if (events[i]->getName() == "kbd_SHIFT_RIGHT_down")
      {  _virtualToRoomSpace = CoordinateFrame(Vector3(0.1,0,0)) * _virtualToRoomSpace;
      }
      else if (events[i]->getName() == "kbd_SHIFT_UP_down")
      {  _virtualToRoomSpace = CoordinateFrame(Vector3(0,0.1,0)) * _virtualToRoomSpace;
      }
      else if (events[i]->getName() == "kbd_SHIFT_DOWN_down")
      {  _virtualToRoomSpace = CoordinateFrame(Vector3(0,-0.1,0)) * _virtualToRoomSpace;
      }


      // Some printouts for other events, just to show how to access other types of event data
      else if (events[i]->getName() == "kbd_SPACE_down")
      {  cout << "Pressed the space key." << endl;
      }
      else if (events[i]->getName() == "Wand_Btn1_down")
      {  cout << "Wand btn 1 pressed." << endl;
      }
      else if (events[i]->getName() == "Wand_Btn2_down")
      {  cout << "Wand btn 2 pressed." << endl;
      }
      else if (events[i]->getName() == "Wand_Btn3_down")
      {  cout << "Wand btn 3 pressed." << endl;
      }
      else if (events[i]->getName() == "Wand_Btn4_down")
      {  cout << "Wand btn 4 pressed." << endl;
      }
      else if (events[i]->getName() == "Wand_Btn6_down")
      {  cout << "Wand btn 6 pressed." << endl;
      }
      else if (events[i]->getName() == "Wand_Btn5_down")
      {  cout << "Wand joystick btn pressed." << endl;
      }
      else if (events[i]->getName() == "Wand_Btn6_down")
      {  cout << "Wand trigger btn pressed." << endl;
      }
      else if (events[i]->getName() == "WandJoy_X")
      {  cout << "Wand Joystick X = " << events[i]->get1DData() << endl;
      }
      else if (events[i]->getName() == "WandJoy_Y")
      {  cout << "Wand Joystick Y = " << events[i]->get1DData() << endl;
      }
      else if (events[i]->getName() == "Mouse_Pointer")
      {  static Vector2 lastPos;

         if (events[i]->get2DData() != lastPos)
         {  //  cout << "New mouse position = " << events[i]->get2DData() << endl;
            lastPos = events[i]->get2DData();
         }
      }
      else if (events[i]->getName() == "Mouse_Left_Btn_down")
      {  //cout << "Mouse left btn pressed at position " << events[i]->get2DData() << endl;
      }
      else if (beginsWith(events[i]->getName(), "kbd_"))
      {  cout << "Keyboard event: " << events[i]->getName() << endl;
      }
      else if (events[i]->getName() == "SpaceNav_Trans")
      {  cout << "Keyboard event: " << events[i]->getName()<< events[i]->get3DData() << endl;
      }
      else if (events[i]->getName() == "SpaceNav_Rot")
      {  cout << "Keyboard event: " << events[i]->getName()<< events[i]->get3DData() << endl;
      }
      else if (beginsWith(events[i]->getName(), "TNG_An"))
      {  cout << events[i]->getName() << "  " << events[i]->get1DData() << endl;
      }
      else if (events[i]->getName() == "SynchedTime")
      {  continue;
      }
      else
      {  // This will print out the names of all events, but can be too
         // much if you are getting several tracker updates per frame.
         // Uncomment this to see everything..
         cout << events[i]->getName() << endl;
      }

      // For debugging tracker coordinate systems, it can be useful to print out
      // tracker positions, like this:
      //
      if (events[i]->getName() == "Test_Tracker")
      {  cout << events[i]->getName() << " " << events[i]->getCoordinateFrameData().translation << endl;
      }
    }
  }

  void doGraphics(RenderDevice *rd)
  {
    // Load a font for the fps display, findVRG3DDataFile looks first
    // in the current directory and then in $G/src/VRG3D/share/
    //
    while(glGetError() != GL_NO_ERROR)
    {  std::cout<<"Flushing gl errors"<<std::endl;
    }

    if (_font.isNull())
    {  std::string fontfile = VRApp::findVRG3DDataFile("eurostyle.fnt");

       ///if ( fileExists( fontfile ))
       if ( FileSystem::exists( fontfile ))
       {   std::cout << fontfile << std::endl;
           _font = GFont::fromFile( fontfile );
       }
    }

    // Draw labeled axes for all the Tracker events we have received, except, skip the
    // Head_Tracker because drawing axes right on top of the eyes would block our view
    // of everything else.
    double axesSize = 0.15;
    Array<std::string> trackerNames = _trackerFrames.getKeys();

    for (int i=0;i<trackerNames.size();i++)
    {  CoordinateFrame trackerFrame = _trackerFrames[trackerNames[i]];

       if (trackerNames[i] != "Head_Tracker")
       {  Draw::axes( trackerFrame, rd,
                      Color3::red(), Color3::green(), Color3::blue(),
                      axesSize);

        if (_font.notNull())
        {  rd->pushState();
           rd->disableLighting();
           CoordinateFrame textframe = trackerFrame * CoordinateFrame(Vector3(1.1*axesSize, 0, 0));
           _font->draw3D(rd, trackerNames[i], textframe, 0.25*axesSize, Color3::white());
           rd->popState();
        }
      }

      if (_font.notNull())
      {
        // This draws the position of the tracker on the screen
        rd->push2D();
        rd->disableLighting();

        std::string s = format( "%s: %.2f, %.2f, %.2f", trackerNames[i].c_str(),
                                trackerFrame.translation[0],
                                trackerFrame.translation[1],
                                trackerFrame.translation[2]);

        _font->draw2D(rd, s, Vector2(25,50 + 25*i), 12, Color3::white());
        rd->pop2D();
      }
    }

    // Drawing the projection of each eye onto the filmplane is often
    // a good way to debug head tracking
    //
    Plane filmplane = Plane(getCamera()->tile.topLeft, getCamera()->tile.botLeft, getCamera()->tile.topRight);
    Vector3 norm = filmplane.normal();

    Vector3 leftEye = getCamera()->getLeftEyeFrame().translation;
    Ray rl = Ray::fromOriginAndDirection(leftEye, -norm);
    Vector3 leftEyeScreen = rl.intersection(filmplane);

    if (!leftEyeScreen.isFinite())
    {
      // projection didn't work, try reversing ray direction
      //
      rl = Ray::fromOriginAndDirection(leftEye, norm);
      leftEyeScreen = rl.intersection(filmplane);
    }

    if (leftEyeScreen.isFinite())
    {
      Draw::sphere(Sphere(leftEyeScreen, 0.015), rd, Color3::red(), Color4::clear());
    }

    Vector3 rightEye = getCamera()->getRightEyeFrame().translation;
    Ray rr = Ray::fromOriginAndDirection(rightEye, -norm);
    Vector3 rightEyeScreen = rr.intersection(filmplane);

    if (!rightEyeScreen.isFinite())
    {
      // projection didn't work, try reversing ray direction
      rr = Ray::fromOriginAndDirection(rightEye, norm);
      rightEyeScreen = rr.intersection(filmplane);
    }

    if (rightEyeScreen.isFinite())
    {  Draw::sphere( Sphere( rightEyeScreen, 0.015 ),
                     rd, Color3::green(), Color4::clear() );
    }

    // This code draws the frames per second on the screen
    //
    if (_font.notNull())
    {  rd->push2D();
       ///std::string msg = format("%3d fps", iRound(rd->frameRate()));
       std::string msg = format("%3d fps", iRound(rd->stats().frameRate));
       _font->draw2D(rd, msg, Vector2(25,25), 12, Color3(0.61, 0.72, 0.92));
       rd->pop2D();
    }

    // The tracker frames above are drawn with the object to world
    // matrix set to the identity because tracking data comes into the
    // system in the Room Space coordinate system.  Room Space is tied
    // to the dimensions of the room and the projection screen within
    // the room, thus it never changes as your program runs.  However,
    // it is often convenient to move objects around in a virtual
    // space that can change relative to the screen.  For these
    // objects, we put a virtual to room space transform on the OpenGL
    // matrix stack before drawing them, as is done here..
    //
    rd->pushState();
    rd->setObjectToWorldMatrix(_virtualToRoomSpace);

    // This draws a simple piece of geometry using G3D::Draw at the
    // origin of Virtual Space.
    //
    Draw::axes( CoordinateFrame(), rd,
                Color3::red(), Color3::green(), Color3::blue(), 0.25 );

    rd->popState();
  }

protected:
  Table<std::string, CoordinateFrame> _trackerFrames;
  GFontRef          _font;
  MouseToTrackerRef _mouseToTracker;
  CoordinateFrame   _virtualToRoomSpace;
};




int main( int argc, char **argv )
{
  // The first argument to the program tells us which of the known VR
  // setups to start
  std::string setupStr;

  if (argc >= 2)
  {  setupStr = std::string(argv[1]);
  }

  // This opens up the graphics window, and starts connections to
  // input devices, but doesn't actually start rendering yet.
  MyVRApp *app = new MyVRApp(setupStr);

  // This starts the rendering/input processing loop
  app->run();

  return 0;
}


//
////////////////////  end  common/vrg3d/demo/vrg3d_demo.cpp  ///////////////////
