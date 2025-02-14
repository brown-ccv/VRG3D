/**
 @file GLG3D.h

 This header includes all of the GLG3D libraries in 
 appropriate namespaces.

 @maintainer Morgan McGuire, http://graphics.cs.williams.edu

 @created 2002-08-07
 @edited  2010-04-04

 Copyright 2000-2010, Morgan McGuire.
 All rights reserved.
*/

#ifndef G3D_GLG3D_H
#define G3D_GLG3D_H

#include "G3D/G3D.h"

// Set up the linker on Windows
#ifdef _MSC_VER

#   pragma comment(lib, "ole32")
#   pragma comment(lib, "opengl32")
#   pragma comment(lib, "glu32")
#   pragma comment(lib, "shell32") // for drag drop

// FFMPEG:
#ifndef G3D_NO_FFMPEG
//#   pragma comment(lib, "mingwrt")
#   pragma comment(lib, "avutil")
#   pragma comment(lib, "avcodec")
#   pragma comment(lib, "avformat")
#endif

#   ifdef _DEBUG
        // Don't link against G3D when building G3D itself.
#       ifndef G3D_BUILDING_LIBRARY_DLL
#           pragma comment(lib, "GLG3Dd")
#       endif
#   else
        // Don't link against G3D when building G3D itself.
#       ifndef G3D_BUILDING_LIBRARY_DLL
#           pragma comment(lib, "GLG3D")
#       endif
#   endif
#endif

#include "GLG3D/glheaders.h"
#include "GLG3D/glcalls.h"
#include "GLG3D/getOpenGLState.h"
#include "GLG3D/Texture.h"
#include "GLG3D/glFormat.h"
#include "GLG3D/Milestone.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/VertexBuffer.h"
#include "GLG3D/VertexRange.h"
#include "GLG3D/GFont.h"
#include "GLG3D/SkyParameters.h"
#include "GLG3D/Sky.h"
#include "GLG3D/UserInput.h"
#include "GLG3D/FirstPersonManipulator.h"
#include "GLG3D/Draw.h"
#include "GLG3D/tesselate.h"
#include "GLG3D/GApp.h"
#include "GLG3D/Surface.h"
#include "GLG3D/IFSModel.h"
#include "GLG3D/MD2Model.h"
#include "GLG3D/MD3Model.h"
#include "GLG3D/shadowVolume.h"
#include "GLG3D/OSWindow.h"
#include "GLG3D/SDLWindow.h"
#include "GLG3D/edgeFeatures.h"
#include "GLG3D/Shader.h"
#include "GLG3D/GLCaps.h"
#include "GLG3D/Shape.h"
#include "GLG3D/Renderbuffer.h"
#include "GLG3D/Framebuffer.h"
#include "GLG3D/Widget.h"
#include "GLG3D/ThirdPersonManipulator.h"
#include "GLG3D/GConsole.h"
#include "GLG3D/BSPMAP.h"
#include "GLG3D/GKey.h"
#include "GLG3D/ArticulatedModel.h"
#include "GLG3D/Material.h"
#include "GLG3D/SuperShader.h"
#include "GLG3D/GaussianBlur.h"
#include "GLG3D/SuperSurface.h"
#include "GLG3D/DirectionHistogram.h"
#include "GLG3D/SuperBSDF.h"
#include "GLG3D/Component.h"
#include "GLG3D/Film.h"
#include "GLG3D/Tri.h"
#include "GLG3D/TriTree.h"
#include "GLG3D/Profiler.h"

#include "GLG3D/GuiTheme.h"
#include "GLG3D/GuiButton.h"
#include "GLG3D/GuiWindow.h"
#include "GLG3D/GuiCheckBox.h"
#include "GLG3D/GuiControl.h"
#include "GLG3D/GuiContainer.h"
#include "GLG3D/GuiLabel.h"
#include "GLG3D/GuiPane.h"
#include "GLG3D/GuiRadioButton.h"
#include "GLG3D/GuiSlider.h"
#include "GLG3D/GuiTextBox.h"
#include "GLG3D/GuiMenu.h"
#include "GLG3D/GuiDropDownList.h"
#include "GLG3D/GuiNumberBox.h"
#include "GLG3D/GuiFunctionBox.h"
#include "GLG3D/GuiTextureBox.h"
#include "GLG3D/GuiTabPane.h"
#include "GLG3D/FileDialog.h"
#include "GLG3D/IconSet.h"

#include "GLG3D/BackgroundWidget.h"
#include "GLG3D/UprightSplineManipulator.h"
#include "GLG3D/CameraControlWindow.h"
#include "GLG3D/DeveloperWindow.h"
#include "GLG3D/VideoRecordDialog.h"

#include "GLG3D/VideoInput.h"
#include "GLG3D/VideoOutput.h"
#include "GLG3D/ShadowMap.h"
#include "GLG3D/GBuffer.h"

#include "GLG3D/Discovery.h"
#include "GLG3D/GEntity.h"

#ifdef G3D_OSX
#include "GLG3D/CarbonWindow.h"
#endif

#ifdef G3D_WIN32
#include "GLG3D/Win32Window.h"
#include "GLG3D/DXCaps.h"
#endif

#ifdef G3D_LINUX
#include "GLG3D/X11Window.h"
#endif

#ifdef G3D_OSX
#include "GLG3D/SDLWindow.h"
#endif

#endif
