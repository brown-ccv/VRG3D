cmake_minimum_required (VERSION 2.6)
set (CMAKE_VERBOSE_MAKEFILE TRUE)

set (SOURCEFILES ConfigVal.cpp CovarianceMatrix.cpp DefaultMouseNav.cpp 
  Fsa.cpp FsaHelper.cpp GfxMgr.cpp HaloLineSet.cpp LoadingScreen.cpp
  ObjParser.cpp plyfile.c PlyParser.cpp Shadows.cpp SMesh.cpp SpaceNavHCI.cpp
  StringUtils.cpp TexPerFrameSMesh.cpp TextFileReader.cpp TouchBackground.cpp
  TouchButton.cpp TouchManager.cpp TouchWidget.cpp ViewerHCI.cpp EventMgr.cpp Simple3DAudioClient.cpp VRML2Parser.cpp)

set (HEADERFILES CommonInc.H ConfigVal.H CovarianceMatrix.H DefaultMouseNav.H EventFilter.H Fsa.H FsaHelper.H GfxMgr.H GfxMgrCallbacks.H HaloLineSet.H LoadingScreen.H Markers.H ObjParser.H ply.h PlyParser.H Shadows.H SMesh.H SpaceNavHCI.H StringUtils.H TexPerFrameSMesh.H TextFileReader.H TouchBackground.H TouchButton.H TouchButtonCallback.H TouchManager.H TouchWidget.H VRBase.H VRBaseApp.H  VRML2Parser.H ViewerHCI.H EventMgr.H Simple3DAudioClient.H)

source_group("Header Files" FILES ${HEADERFILES})

include_directories(
  .
  ../vrg3d
)

add_library (vrbase ${SOURCEFILES} ${HEADERFILES})


