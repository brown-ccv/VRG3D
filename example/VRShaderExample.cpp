//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


/*
 * \author Daniel Keefe (dfk)
 *
 * \file  vrg3d-demo.cpp
 *
 */


#include <vrg3d/VRG3D.h>

#include <time.h>

#include "VRShaderExample.h"
using namespace G3D;


/** This is a sample VR application using the VRG3D library.  Two key
    methods are filled in here: doGraphics() and doUserInput().  The
    code in these methods demonstrates how to draw graphics and
    respond to input from the mouse, the keyboard, 6D trackers, and VR
    wand buttons.
*/
class VRShaderExample : public VRApp
{
public:
  VRShaderExample(const std::string &mySetup, G3D::PrimitiveType geoInputType = PrimitiveType::TRIANGLES, G3D::PrimitiveType geoOutputType = PrimitiveType::TRIANGLE_STRIP) : VRApp() {
    // initialize the VRApp
    init(mySetup);

    _mouseToTracker = new MouseToTracker(getCamera(), 2);

    _virtualToRoomSpace = CoordinateFrame();

    _clearColor = Color3(0.75, 0.75, 0.75);
   // _clearColor = Color3(1.0, 1.0, 1.0);

    _vertices.append(G3D::Vector3(-0.5, 0.0, 0.0));
    _vertices.append(G3D::Vector3(-0.5, 0.0, 0.0));
    _vertices.append(G3D::Vector3(0.5, 0.0, 0.0));
    _vertices.append(G3D::Vector3(0.5, 0.0, 0.0));
    _vertices.append(G3D::Vector3(0.0, 0.5, 0.2));
    _vertices.append(G3D::Vector3(0.0, 0.5, 0.2));
    _vertices.append(G3D::Vector3(0.0, -0.5, 0.2));
    _vertices.append(G3D::Vector3(0.0, -0.5, 0.2));
    _vertices.append(G3D::Vector3(-0.5, 0.2, 0.2));
    _vertices.append(G3D::Vector3(-0.5, 0.2, 0.2));

    for(int i = 0; i < 10; i++) {
      cout << "_vertices[" << i << "] = " << _vertices[i] << endl;
    }

    cout << "RAND_MAX = " << RAND_MAX << endl;

    ///srand(time(NULL));    
    ///srand(44497);    
    srand((long)time(NULL));    

    Color3 color((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
    cout << "color = " << color << endl;
    
    for(int i = 0; i < 10; i++) {
      _indices.append(i);
      int t = i % 2;
      _texCoords.append(Vector2(0.0, t));
      _normals.append(G3D::Vector3());
      _colors.append(color);
    }

    cout << "vertices.size() = " << _vertices.size() << endl;
    computeNormal(_normals[0], _vertices[0], _vertices[2], _vertices[2], _vertices[4]);
    computeNormal(_normals[1], _vertices[1], _vertices[3], _vertices[3], _vertices[5]);
    for(int i = 2; i < _vertices.size() - 2; i++) {
      computeNormal(_normals[i], _vertices[i-2], _vertices[i], _vertices[i], _vertices[i+2]);
    }
    computeNormal(_normals[8], _vertices[4], _vertices[6], _vertices[6], _vertices[8]);
    computeNormal(_normals[9], _vertices[5], _vertices[7], _vertices[7], _vertices[9]);

    for(int i = 0; i < 10; i++) {
      cout << "_normals[" << i << "] = " << _normals[i] << endl;
    }

    int sizeNeeded = 8 + sizeof(Vector3)*_vertices.size() + 8 + sizeof(Vector3)*_normals.size() + 8 + sizeof(Vector2)*_texCoords.size() + 8 + sizeof(Color3)*_colors.size();  
    m_varArea = VARArea::create(sizeNeeded, VARArea::WRITE_ONCE);
    if(m_varArea.isNull()) {
      cerr << "Out of VARArea room" << endl;
    } else {
      m_varArea->reset();
      m_vertexVAR = VAR(_vertices, m_varArea);
      m_normalVAR = VAR(_normals, m_varArea);
      m_texCoordVAR = VAR(_texCoords, m_varArea);
      m_colorVAR = VAR(_colors, m_varArea);
    }

    int maxGeometryOutputVertices;
//  if(geoInputType != PrimitiveType::TRIANGLES || geoOutputType != PrimitiveType::TRIANGLE_STRIP) {
      // G3D assumes that geometry shaders take TRIANGLES as input and output TRIANGLE_STRIP
      // If we want anything different, we have to hack our way around this limitation by
      // setting the max # of vertices to -1


      // ALERT!  You'd need to change the PrimitiveType arguments to openGL enums anyway
      //_shader = Shader::fromFiles("shader.vert", "shader.geom", "shader.frag", -1);
      //glProgramParameteriEXT(_shader->glProgramObject(), GL_GEOMETRY_INPUT_TYPE_EXT, geoInputType);   
      //glProgramParameteriEXT(_shader->glProgramObject(), GL_GEOMETRY_OUTPUT_TYPE_EXT, geoOutputType); 
      glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, &maxGeometryOutputVertices);
      cout << "maxVertices = " << maxGeometryOutputVertices << endl;
      _shader = Shader::fromFiles("shader.vert", "shader.frag");
      //_shader = Shader::fromFiles("shader.vert", "shader.geom", "shader.frag", maxGeometryOutputVertices);
      cout << "Shader log: " << _shader->messages() << endl;;
      //debugAssertM(maxGeometryOutputVertices <= glGetInteger(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT), "maxGeometryOutputVertices exceeds GPU capabilities.");
      //glProgramParameteriEXT(_shader->glProgramObject(), GL_GEOMETRY_VERTICES_OUT_EXT, maxGeometryOutputVertices);
    //} else {
    //glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, &maxGeometryOutputVertices);
    //_shader = Shader::fromFiles("../../../shader.vert", "../../../shader.geom", "../../../shader.frag", maxGeometryOutputVertices);
    //}
  }

  virtual ~VRShaderExample() {}

  void computeNormal(Vector3 &n, Vector3 vert1, Vector3 vert2, Vector3 vert3, Vector3 vert4) {

   //normal =  ((vert2 - vert1) + (vert4 - vert3)).unit();

    Vector3 vec1(vert2.x - vert1.x, vert2.y - vert1.y, vert2.z - vert1.z);
    float mag = sqrt(vec1.x*vec1.x + vec1.y *vec1.y + vec1.z*vec1.z);
    if(mag != 0) {
      vec1.x = vec1.x/mag;
      vec1.y = vec1.y/mag;
      vec1.z = vec1.z/mag;
    }

    Vector3 vec2(vert4.x - vert3.x, vert4.y - vert3.y, vert4.z - vert3.z);
    mag = sqrt(vec2.x*vec2.x + vec2.y *vec2.y + vec2.z*vec2.z);
    if(mag != 0) {
      vec2.x = vec2.x/mag;
      vec2.y = vec2.y/mag;
      vec2.z = vec2.z/mag;
    }

    n.x = (vec1.x + vec2.x)/2;
    n.y = (vec1.y + vec2.y)/2;
    n.z = (vec1.z + vec2.z)/2;
  }

  void doUserInput(Array<VRG3D::EventRef> &events) {

    for(int i = 0; i < events.size(); i++) {
      if(events[i]->getName() == "kbd_UP_down") {
    // Rotate the object vertically up
    std::cout << "Up key pressed" << std::endl;
    _virtualToRoomSpace = CoordinateFrame(Matrix3::fromAxisAngle(Vector3(1,0,0), toRadians(-5.0))) * _virtualToRoomSpace;
      } else if(events[i]->getName() == "kbd_DOWN_down") {
    // Rotate the object vertically down
    std::cout << "Down key pressed" << std::endl;
    _virtualToRoomSpace = CoordinateFrame(Matrix3::fromAxisAngle(Vector3(1,0,0), toRadians(5.0))) * _virtualToRoomSpace;
      } else if(events[i]->getName() == "kbd_LEFT_down") {
    // Rotate the object horizontally left
    std::cout << "Left key pressed" << std::endl;
    _virtualToRoomSpace = CoordinateFrame(Matrix3::fromAxisAngle(Vector3(0,1,0), toRadians(-5.0))) * _virtualToRoomSpace;
      } else if(events[i]->getName() == "kbd_RIGHT_down") {
    // Rotate the object horizontally right
    std::cout << "Right key pressed" << std::endl;
    _virtualToRoomSpace = CoordinateFrame(Matrix3::fromAxisAngle(Vector3(0,1,0), toRadians(5.0))) * _virtualToRoomSpace;
      }


    }

    /*
    // MouseToTracker is a really helpful class for testing out VR
    // interactions from the desktop.  This call makes it respond to
    // mouse events and generate new events as if it were a 6DOF
    // tracking device.  We add the new events to the event queue and
    // process them as usual.

    Array<VRG3D::EventRef> newEvents;
    _mouseToTracker->doUserInput(events, newEvents);
    events.append(newEvents);


    for (int i=0;i<events.size();i++) {

      if (events[i]->getName() == "kbd_ESC_down") {
    while(glGetError() != GL_NO_ERROR){std::cout<<"Flushing gl errors"<<std::endl;}
        exit(0);
      }

      // Save all the tracker events that come in so we can use them in the doGraphics routine
      else if (endsWith(events[i]->getName(), "_Tracker")) {
        if (_trackerFrames.containsKey(events[i]->getName())) {
          _trackerFrames[events[i]->getName()] = events[i]->getCoordinateFrameData();
        }
        else {
          _trackerFrames.set(events[i]->getName(), events[i]->getCoordinateFrameData());
        }
      }

      // Respond to events to do some simple navigation
      else if (events[i]->getName() == "kbd_LEFT_down") {
        _virtualToRoomSpace = CoordinateFrame(Matrix3::fromAxisAngle(Vector3(0,1,0), toRadians(5.0))) * _virtualToRoomSpace;
      }
      else if (events[i]->getName() == "kbd_RIGHT_down") {
        _virtualToRoomSpace = CoordinateFrame(Matrix3::fromAxisAngle(Vector3(0,1,0), toRadians(-5.0))) * _virtualToRoomSpace;
      }
      else if (events[i]->getName() == "kbd_UP_down") {
        _virtualToRoomSpace = CoordinateFrame(Matrix3::fromAxisAngle(Vector3(1,0,0), toRadians(5.0))) * _virtualToRoomSpace;
      }
      else if (events[i]->getName() == "kbd_DOWN_down") {
        _virtualToRoomSpace = CoordinateFrame(Matrix3::fromAxisAngle(Vector3(1,0,0), toRadians(-5.0))) * _virtualToRoomSpace;
      }
      else if (events[i]->getName() == "kbd_SHIFT_LEFT_down") {
        _virtualToRoomSpace = CoordinateFrame(Vector3(-0.1,0,0)) * _virtualToRoomSpace;
      }
      else if (events[i]->getName() == "kbd_SHIFT_RIGHT_down") {
        _virtualToRoomSpace = CoordinateFrame(Vector3(0.1,0,0)) * _virtualToRoomSpace;
      }
      else if (events[i]->getName() == "kbd_SHIFT_UP_down") {
        _virtualToRoomSpace = CoordinateFrame(Vector3(0,0.1,0)) * _virtualToRoomSpace;
      }
      else if (events[i]->getName() == "kbd_SHIFT_DOWN_down") {
        _virtualToRoomSpace = CoordinateFrame(Vector3(0,-0.1,0)) * _virtualToRoomSpace;
      }


      // Some printouts for other events, just to show how to access other types of event data
      else if (events[i]->getName() == "kbd_SPACE_down") {
        cout << "Pressed the space key." << endl;
      }
      else if (events[i]->getName() == "Wand_Btn1_down") {
        cout << "Wand btn 1 pressed." << endl;
      }
      else if (events[i]->getName() == "Wand_Btn2_down") {
        cout << "Wand btn 2 pressed." << endl;
      }
      else if (events[i]->getName() == "Wand_Btn3_down") {
        cout << "Wand btn 3 pressed." << endl;
      }
      else if (events[i]->getName() == "Wand_Btn4_down") {
        cout << "Wand btn 4 pressed." << endl;
      }
      else if (events[i]->getName() == "Wand_Btn6_down") {
        cout << "Wand btn 6 pressed." << endl;
      }
      else if (events[i]->getName() == "Wand_Btn5_down") {
        cout << "Wand joystick btn pressed." << endl;
      }
      else if (events[i]->getName() == "Wand_Btn6_down") {
        cout << "Wand trigger btn pressed." << endl;
      }
      else if (events[i]->getName() == "WandJoy_X") {
        cout << "Wand Joystick X = " << events[i]->get1DData() << endl;
      }
      else if (events[i]->getName() == "WandJoy_Y") {
        cout << "Wand Joystick Y = " << events[i]->get1DData() << endl;
      }
      else if (events[i]->getName() == "Mouse_Pointer") {
        static Vector2 lastPos;
        if (events[i]->get2DData() != lastPos) {
        //  cout << "New mouse position = " << events[i]->get2DData() << endl;
          lastPos = events[i]->get2DData();
        }
      }
      else if (events[i]->getName() == "Mouse_Left_Btn_down") {
        //cout << "Mouse left btn pressed at position " << events[i]->get2DData() << endl;
      }
      else if (beginsWith(events[i]->getName(), "kbd_")) {
        cout << "Keyboard event: " << events[i]->getName() << endl;
      }
      else if (events[i]->getName() == "SpaceNav_Trans") {
        cout << "Keyboard event: " << events[i]->getName()<< events[i]->get3DData() << endl;
      }
      else if (events[i]->getName() == "SpaceNav_Rot") {
        cout << "Keyboard event: " << events[i]->getName()<< events[i]->get3DData() << endl;
      }
      else if (beginsWith(events[i]->getName(), "TNG_An")) {
        cout << events[i]->getName() << "  " << events[i]->get1DData() << endl;
      }
    else if (events[i]->getName() == "SynchedTime"){
        continue;
}
      else {
        // This will print out the names of all events, but can be too
        // much if you are getting several tracker updates per frame.
        // Uncomment this to see everything..
          cout << events[i]->getName() << endl;
      }



      // For debugging tracker coordinate systems, it can be useful to print out
      // tracker positions, like this:
      if (events[i]->getName() == "Test_Tracker") {
        cout << events[i]->getName() << " " << events[i]->getCoordinateFrameData().translation << endl;
      }

    }*/
  }

  void doGraphics(RenderDevice *rd) {

    rd->setShader(_shader);
    rd->setCullFace(RenderDevice::CULL_NONE);

    rd->setObjectToWorldMatrix(_virtualToRoomSpace);  
    //rd->setProjectionAndCameraMatrix(GCamera(Matrix4::perspectiveProjection(-1.0, 1.0, -1.0, 1.0, 1.4, -1.4), _virtualToRoomSpace));
//    rd->setColor(Color3(0.0, 0.0, 0.0));
    rd->setDepthTest(RenderDevice::DEPTH_LEQUAL);
    rd->setDepthWrite(true);
    rd->setDepthRange(0.0, 1.0);
    rd->beginIndexedPrimitives();
    rd->setVertexArray(m_vertexVAR);
    rd->setNormalArray(m_normalVAR);
    rd->setColorArray(m_colorVAR);
    rd->setTexCoordArray(0, m_texCoordVAR);
    rd->sendIndices(PrimitiveType::TRIANGLE_STRIP, _indices);
    rd->endIndexedPrimitives();
  
/*
    rd->beginPrimitive(PrimitiveType::TRIANGLE_STRIP);
      rd->setNormal(_normals[2]);
      rd->setTexCoord(0, Vector2(0.0, 0.0));
      rd->sendVertex(_vertices[2]);

      rd->setNormal(_normals[0]);
      rd->setTexCoord(0, Vector2(0.0, 0.0));
      rd->sendVertex(_vertices[0]);

      rd->setNormal(_normals[3]);
      rd->setTexCoord(0, Vector2(0.0, 1.0));
      rd->sendVertex(_vertices[3]);

      rd->setNormal(_normals[1]);
      rd->setTexCoord(0, Vector2(0.0, 1.0));
      rd->sendVertex(_vertices[1]);
    rd->endPrimitive();

    rd->beginPrimitive(PrimitiveType::TRIANGLE_STRIP);
      rd->setNormal(_normals[4]);
      rd->setTexCoord(0, Vector2(0.0, 0.0));
      rd->sendVertex(_vertices[4]);

      rd->setNormal(_normals[2]);
      rd->setTexCoord(0, Vector2(0.0, 0.0));
      rd->sendVertex(_vertices[2]);

      rd->setNormal(_normals[5]);
      rd->setTexCoord(0, Vector2(0.0, 1.0));
      rd->sendVertex(_vertices[5]);

      rd->setNormal(_normals[3]);
      rd->setTexCoord(0, Vector2(0.0, 1.0));
      rd->sendVertex(_vertices[3]);
    rd->endPrimitive();

    rd->beginPrimitive(PrimitiveType::TRIANGLE_STRIP);
      rd->setNormal(_normals[6]);
      rd->setTexCoord(0, Vector2(0.0, 0.0));
      rd->sendVertex(_vertices[6]);

      rd->setNormal(_normals[4]);
      rd->setTexCoord(0, Vector2(0.0, 0.0));
      rd->sendVertex(_vertices[4]);

      rd->setNormal(_normals[7]);
      rd->setTexCoord(0, Vector2(0.0, 1.0));
      rd->sendVertex(_vertices[7]);

      rd->setNormal(_normals[5]);
      rd->setTexCoord(0, Vector2(0.0, 1.0));
      rd->sendVertex(_vertices[5]);
    rd->endPrimitive();

    rd->beginPrimitive(PrimitiveType::TRIANGLE_STRIP);
      rd->setNormal(_normals[8]);
      rd->setTexCoord(0, Vector2(0.0, 0.0));
      rd->sendVertex(_vertices[8]);

      rd->setNormal(_normals[6]);
      rd->setTexCoord(0, Vector2(0.0, 0.0));
      rd->sendVertex(_vertices[6]);

      rd->setNormal(_normals[9]);
      rd->setTexCoord(0, Vector2(0.0, 1.0));
      rd->sendVertex(_vertices[9]);

      rd->setNormal(_normals[7]);
      rd->setTexCoord(0, Vector2(0.0, 1.0));
      rd->sendVertex(_vertices[7]);
    rd->endPrimitive();
*/
    //rd->setTexture(0, Texture::fromFile("texture.jpg"));

    //rd->setShadeMode(RenderDevice::SHADE_SMOOTH);
/*
    rd->beginPrimitive(PrimitiveType::TRIANGLES);
      rd->setTexCoord(0, Vector3(0.15, 0.65, 0.0));
      rd->setColor(Color3(1.0, 0.0, 0.0));
      rd->sendVertex(Vector3(-0.25, -0.25, 0.0));

      rd->setTexCoord(0, Vector3(0.65, 0.65, 0.0));
      rd->setColor(Color3(0.0, 1.0, 0.0));
      rd->sendVertex(Vector3(0.25, -0.25, 0.0));

      rd->setTexCoord(0, Vector3(0.15, 0.15, 0.0));
      rd->setColor(Color3(0.0, 0.0, 1.0));
      rd->sendVertex(Vector3(-0.25, 0.25, 0.0));
    rd->endPrimitive();

    rd->beginPrimitive(PrimitiveType::TRIANGLES);
      rd->setTexCoord(0, Vector3(0.65, 0.65, 0.0));
      rd->setColor(Color3(0.0, 1.0, 0.0));
      rd->sendVertex(Vector3(0.25, -0.25, 0.0));

      rd->setTexCoord(0, Vector3(0.65, 0.15, 0.0));
      rd->setColor(Color3(1.0, 1.0, 0.0));
      rd->sendVertex(Vector3(0.25, 0.25, 0.0));

      rd->setTexCoord(0, Vector3(0.15, 0.15, 0.0));
      rd->setColor(Color3(0.0, 0.0, 1.0));
      rd->sendVertex(Vector3(-0.25, 0.25, 0.0));
    rd->endPrimitive();
*/
/*
    // Load a font for the fps display, findVRG3DDataFile looks first
    // in the current directory and then in $G/src/VRG3D/share/
    while(glGetError() != GL_NO_ERROR){std::cout<<"Flushing gl errors"<<std::endl;}
    if (_font.isNull()) {
      std::string fontfile = VRApp::findVRG3DDataFile("eurostyle.fnt");
      if (fileExists(fontfile)) {
    std::cout<<fontfile<<std::endl;
        _font = GFont::fromFile(fontfile);
      }
    }

    // Draw labeled axes for all the Tracker events we have received, except, skip the
    // Head_Tracker because drawing axes right on top of the eyes would block our view
    // of everything else.
    double axesSize = 0.15;
    Array<std::string> trackerNames = _trackerFrames.getKeys();
    for (int i=0;i<trackerNames.size();i++) {
      CoordinateFrame trackerFrame = _trackerFrames[trackerNames[i]];
      if (trackerNames[i] != "Head_Tracker") {
        Draw::axes(trackerFrame, rd, Color3::red(), Color3::green(), Color3::blue(), axesSize);
        if (_font.notNull()) {
          rd->pushState();
          rd->disableLighting();
          CoordinateFrame textframe = trackerFrame * CoordinateFrame(Vector3(1.1*axesSize, 0, 0));
          _font->draw3D(rd, trackerNames[i], textframe, 0.25*axesSize, Color3::white());
          rd->popState();
        }
      }
      if (_font.notNull()) {
        // This draws the position of the tracker on the screen
        rd->push2D();
        rd->disableLighting();
        std::string s = format("%s: %.2f, %.2f, %.2f", trackerNames[i].c_str(),
                               trackerFrame.translation[0],
                               trackerFrame.translation[1],
                               trackerFrame.translation[2]);
        _font->draw2D(rd, s, Vector2(25,50 + 25*i), 12, Color3::white());
        rd->pop2D();
      }
    }


    // Drawing the projection of each eye onto the filmplane is often
    // a good way to debug head tracking
    Plane filmplane = Plane(_tile.topLeft, _tile.botLeft, _tile.topRight);
    Vector3 norm = filmplane.normal();

    Vector3 leftEye = _camera->getLeftEyeFrame().translation;
    Ray rl = Ray::fromOriginAndDirection(leftEye, -norm);
    Vector3 leftEyeScreen = rl.intersection(filmplane);
    if (!leftEyeScreen.isFinite()) {
      // projection didn't work, try reversing ray direction
      rl = Ray::fromOriginAndDirection(leftEye, norm);
      leftEyeScreen = rl.intersection(filmplane);
    }
    if (leftEyeScreen.isFinite()) {
      Draw::sphere(Sphere(leftEyeScreen, 0.015), rd, Color3::red(), Color4::clear());
    }

    Vector3 rightEye = _camera->getRightEyeFrame().translation;
    Ray rr = Ray::fromOriginAndDirection(rightEye, -norm);
    Vector3 rightEyeScreen = rr.intersection(filmplane);
    if (!rightEyeScreen.isFinite()) {
      // projection didn't work, try reversing ray direction
      rr = Ray::fromOriginAndDirection(rightEye, norm);
      rightEyeScreen = rr.intersection(filmplane);
    }
    if (rightEyeScreen.isFinite()) {
      Draw::sphere(Sphere(rightEyeScreen, 0.015), rd, Color3::green(), Color4::clear());
    }


    // This code draws the frames per second on the screen
    if (_font.notNull()) {
      rd->push2D();
      //std::string msg = format("%3d fps", iRound(rd->frameRate()));
      std::string msg = "Framerate ?"; 
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
    rd->pushState();
    rd->setObjectToWorldMatrix(_virtualToRoomSpace);
    // This draws a simple piece of geometry using G3D::Draw at the
    // origin of Virtual Space.
    Draw::axes(CoordinateFrame(), rd, Color3::red(), Color3::green(), Color3::blue(), 0.25);

    rd->popState();*/
  }

protected:
  Table<std::string, CoordinateFrame> _trackerFrames;
  GFontRef          _font;
  MouseToTrackerRef _mouseToTracker;
  CoordinateFrame   _virtualToRoomSpace;
  ShaderRef     _shader;
  Array<Vector3>    _vertices;
  Array<Vector3>    _normals;
  Array<int>        _indices;
  Array<Vector2>    _texCoords;
  Array<Color3>     _colors;
  VertexBufferRef   m_varArea;
  VAR           m_vertexVAR;
  VAR           m_normalVAR;
  VAR           m_texCoordVAR;
  VAR           m_colorVAR;
};




int main(int argc, char **argv)
{
  // The first argument to the program tells us which of the known VR
  // setups to start
  std::string setupStr;
  if (argc >= 2) {
    setupStr = std::string(argv[1]);
  }

  // This opens up the graphics window, and starts connections to
  // input devices, but doesn't actually start rendering yet.
  VRShaderExample *shader = new VRShaderExample(setupStr);

  // This starts the rendering/input processing loop
  shader->run();

  return 0;
}

