//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

/* this code generates the checkerboard pattern
to the stencil buffer for stereoscopic masking */

#include "HorizontalStencil.h"

//#include <GL/glut.h>
#include <G3D/G3D.h>
#include <GLG3D/GLG3D.h>
#include <iostream>
using namespace std;

/*  keefe-- Note: there's an issue here with reliably getting a
horizontal stripe pattern on every other line.  This is a general issue
with opengl in that it's hard to write directly to an integer pixel.
The OpenGL Red Book suggests setting gluOrtho2D(0,w,0,h) then for
drawing lines adding 0.5 to the integer value to get it to show up on
the right pixel.  That seems to work here, but not always..  Check
this thread for references: http://www.idevgames.com/forum/archive/index.php/t-1922.html

keefe-- Note: second complicated issue here arrises if the OpenGL two
sided stencil extension is loaded.  G3D loads and enables all
extensions (including this) by default.  I tried to glDisable it, but
it didn't revert to normal stencil behavior, so not sure what's going
on there.  The solution is to check to see if it is enabled, and if
so, draw the checkerboard into both the front and back stencil.  You
also need to set the stencilop approriately for both the front and
back stencils, which is done in VRApp.cpp.
*/

void
horizontal_stencil(int gliWindowWidth, int gliWindowHeight)
{
   GLint gliY;

  // setup projection to work with pixel coordinates
  glViewport(0,0,gliWindowWidth,gliWindowHeight);

  glMatrixMode(GL_MODELVIEW);  //+++ 
  glLoadIdentity();            //+++ 

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // keefe: switched to 0,w,0,h rather than 0,w-1,0,h-1
  gluOrtho2D(0.0,gliWindowWidth,0.0,gliWindowHeight);
  //gluOrtho2D(0.0,gliWindowWidth-1,0.0,gliWindowHeight-1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // clearing and configuring stencil drawing
  glDrawBuffer(GL_BACK);

  if (glIsEnabled(GL_STENCIL_TEST_TWO_SIDE_EXT)) {
    //cout << "two sided stencil" << endl;
    glActiveStencilFaceEXT(GL_FRONT);
  }

  glEnable(GL_STENCIL_TEST);
  glClearStencil(0);
  glClear(GL_STENCIL_BUFFER_BIT);
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE); // colorbuffer is copied to stencil
  glDisable(GL_DEPTH_TEST);
  glStencilFunc(GL_ALWAYS,1,1); // to avoid interaction with stencil content

  //glEnable(GL_LINE_STIPPLE);
  glDisable(GL_LINE_SMOOTH);

  // drawing stencil pattern
  glColor4f(1,1,1,0);// alfa is 0 not to interfere with alpha tests
  for (gliY=1; gliY<gliWindowHeight+1; gliY = gliY + 2) {
    glLineWidth(1);
 
    //if (gliY%2)
    //  glLineStipple(1, 0xAAAA);
    //else
    //  glLineStipple(1, 0x5555);

    glBegin(GL_LINES);
    // keefe: added 0.5's below
    glVertex2f(0, gliY+0.5);
    glVertex2f(gliWindowWidth, gliY+0.5);
    glEnd();
  }


  if (glIsEnabled(GL_STENCIL_TEST_TWO_SIDE_EXT)) {
    glActiveStencilFaceEXT(GL_BACK);

    glEnable(GL_STENCIL_TEST);
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE); // colorbuffer is copied to stencil
    glDisable(GL_DEPTH_TEST);
    glStencilFunc(GL_ALWAYS,1,1); // to avoid interaction with stencil content

    //glEnable(GL_LINE_STIPPLE);
    glDisable(GL_LINE_SMOOTH);

    // drawing stencil pattern
    glColor4f(1,1,1,0);// alfa is 0 not to interfere with alpha tests
    for (gliY=1; gliY<gliWindowHeight+1; gliY = gliY + 2) {
    glLineWidth(1);
 
    //if (gliY%2)
    //  glLineStipple(1, 0xAAAA);
    //else
    //  glLineStipple(1, 0x5555);

    glBegin(GL_LINES);
    // keefe: added 0.5's below
    glVertex2f(0, gliY+0.5);
    glVertex2f(gliWindowWidth, gliY+0.5);
    glEnd();
    }
  }

  glActiveStencilFaceEXT(GL_FRONT);
  glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP); // disabling changes in stencil buffer
  glFlush();
}


