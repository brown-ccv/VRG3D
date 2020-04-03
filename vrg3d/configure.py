#!/usr/bin/env python

#######  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

########################  vrg3d/vrg3d/configure.py  ############################
#
#  Create a configuration paramter file for vrg3d.
#
#  Directory name of the Make utilities for Windows.
#
#...............................................................................


import  os
import  sys
import  platform
import  time
import  string



# Default installation at the Brown CIT.
#
MAKE_VERSION = "g.utility_1.0"
G3D_VERSION  = "g3d_8.01"
VRPN_VERSION = "vrpn_7.26"
GLUT_VERSION = "glut_3.7.6"
SDL_VERSION  = "sdl_1.2.14"

IMPORT_CIT_LINUX = "/research/graphics/tools/linux/import"
IMPORT_CIT_WIN32 = "y:/research/graphics/tools/linux/import"
IMPORT_CIT_OSX   = "/Volumes/graphics/tools/linux/import"

VRPN_CIT_LINUX64 = IMPORT_CIT_LINUX + "/vrpn/vrpn_7.28_gcc4_64"
VRPN_CIT_LINUX   = IMPORT_CIT_LINUX + "/vrpn/" + VRPN_VERSION + "_gcc4"
VRPN_CIT_WIN32   = IMPORT_CIT_WIN32 + "/vrpn/" + VRPN_VERSION + "_cl9"
VRPN_CIT_OSX     = IMPORT_CIT_OSX   + "/vrpn/" + VRPN_VERSION + "_OSX"

G3D_CIT_LINUX64  = IMPORT_CIT_LINUX + "/g3d/" + G3D_VERSION + "_gcc4_64"
G3D_CIT_LINUX    = IMPORT_CIT_LINUX + "/g3d/" + G3D_VERSION + "_gcc4"
G3D_CIT_WIN32    = IMPORT_CIT_WIN32 + "/g3d/" + G3D_VERSION + "_cl9"
G3D_CIT_OSX      = IMPORT_CIT_OSX   + "/g3d/" + G3D_VERSION + "_OSX"

SDL_CIT_WIN32    = IMPORT_CIT_WIN32 + "/sdl/"  + SDL_VERSION  + "_cl8"
GLUT_CIT_WIN32   = IMPORT_CIT_WIN32 + "/glut/" + GLUT_VERSION + "_WIN32"
MAKE_CIT_WIN32   = IMPORT_CIT_WIN32 + "/make/" + MAKE_VERSION

VRG3D_CIT_LINUX  = "/research/graphics/tools/linux/install_linux"
VRG3D_CIT_WIN32  = "y:/research/graphics/tools/linux/install_WIN32"
VRG3D_CIT_OSX    = "/Volumes/graphics/tools/linux/install_OSX"



################################################################################
#
# Display command syntax.
#
def  showUsage( message = "" ):
#
#...............................................................................


   if message:
      printout( "ERROR:  " + message )


   printout( "Usage:  python  configure.py  [options]" )
   printout( "          -g3d      <path>  # Location of g3d" )
   printout( "          -vrpn     <path>  # Location of vrpn" )
   printout( "          -sdl      <path>  # Location of sdl (Windows and Linux only)" )
   printout( "          -glut     <path>  # Location of glut (Windows and Linux only)" )
   printout( "          -library  <path>  # Additional library directories" )
   printout( "          -include  <path>  # Additional include directories" )
   printout( "          -make     <path>  # Location of the Make utilities (Windows only)" )
   printout( "          -vrg3d    <path>  # Location to install vrg3d" )
   printout( "          -use   <options>  # Optional components:  spacenav isense trackd tuio glut" )
   printout( "          -compiler <name>  # Name or path of the C++ compiler" )

   if message:
      sys.exit(-1)


   return



################################################################################
#
#  Simple print compatible with Python versions 2 and 3.
#
def printout( message = "", end = "\n" ):
#
#...............................................................................


   sys.stdout.write( message + end )

   return



################################################################################
#
# Display command syntax.
#
def  oops( message = "" ):
#
#...............................................................................


   printout( "ERROR: ", message)

   sys.exit( -1 )



################################################################################
#
def  setArgumentList():  #0: run; 1: show usage; -1: exit error.

   global  Sdl
   global  Vrpn
   global  G3d
   global  Glut
   global  Make
   global  Vrg3d
   global  Use
   global  Compiler
   global  Library
   global  Include
#
#...............................................................................


   Sdl      = ""
   Vrpn     = ""
   G3d      = ""
   Glut     = ""
   Make     = ""
   Vrg3d    = ""
   Use      = ""
   Compiler = ""
   Library  = []
   Include  = []

   status = 0
   i = 0

   while  i < len( sys.argv ) - 1:
      i  += 1
      key = sys.argv[i]

      if key == "-h"  or  key == "--help":
         showUsage()
         sys.exit( 0 )


      if not key.startswith( "-" ):
         showUsage( "Missing value or keyword (" + key + " )." )


      i += 1

      if i >= len( sys.argv ):
         showUsage( "Missing value on keyword (" + key + " )." )


      value = sys.argv[i]

      if value.startswith( "-" ):
         showUsage( "Missing value on keyword (" + key + " )." )

      elif value[-1:] == "/"  or  value[-1:] == "\\":
         value = value[0:-1]


      if key == "-sdl":
         if Sdl:
            showUsage( "-sdl was set more than once." )


         Sdl = value


      elif key == "-vrpn":
         if Vrpn:
            showUsage( "-vrpn was set more than once." )


         Vrpn = value


      elif key == "-g3d":
         if G3d:
            showUsage( "-g3d was set more than once." )


         G3d = value


      elif key == "-glut":
         if Glut:
            showUsage( "-glut was set more than once." )


         Glut = value


      elif key == "-make":
         if Make:
            showUsage( "-make was set more than once." )


         Make = value


      elif key == "-vrg3d":
         if Vrg3d:
            showUsage( "-vrg3d was set more than once." )


         Vrg3d = value


      elif key == "-use":
         if Use:
            showUsage( "-use was set more than once." )

         Use = value.lower()

         while i + 1 < len( sys.argv )  and              \
               not sys.argv[i + 1].startswith( "-" ):

            i    += 1
            value = sys.argv[i].lower()
            Use  += " " + value


      elif key == "-compiler":
         if Compiler:
            showUsage( "-compiler was set more than once." )


         Compiler = value


      elif key == "-library":
         Library.append( value )

         while i + 1 < len( sys.argv )  and              \
               not sys.argv[i + 1].startswith( "-" ):

            i    += 1
            value = sys.argv[i]

            if value[-1:] == "/"  or  value[-1:] == "\\":
               value = value[0:-1]


            Library.append( value )


      elif key == "-include":
         Include.append( value )

         while i + 1 < len( sys.argv )  and              \
               not sys.argv[i + 1].startswith( "-" ):

            i    += 1
            value = sys.argv[i]

            if value[-1:] == "/"  or  value[-1:] == "\\":
               value = value[0:-1]


            Include.append( value )


      else:
         showUsage( "Unrecognized argument (" + key + " )." )



   #print  " sdl =", Sdl, " g3d =", G3d, " vrpn = ", Vrpn
   #print  " vrg3d =", Vrg3d, " compiler =", Compiler
   #print  " use =", Use


   return  status



################################################################################
#
def  findFile( list, file ):  # Locate a file along a path list.
#
#...............................................................................


   path = ""

   for p in list:
     if os.path.isfile( p + "/" + file ):
        path = p + "/" + file
        break


   return  path



################################################################################
#
def  checkSdl():  # Locate sdl.

   global  Sdl
   global  Platform
   global  Library
   global  Include
#
#...............................................................................


   if Sdl:
      if not os.path.isdir( Sdl ):
         oops( "Invalid Sdl directory:  " + Sdl )

   elif Platform == "WIN32":
      if os.path.isdir( SDL_CIT_WIN32 ):
         Sdl = SDL_CIT_WIN32


   if Platform == "OSX":
      if Sdl:
         oops( "Sdl needs to be installed as a framework." )

      elif not os.path.isfile( "/Library/Frameworks/SDL.framework/Headers/SDL.h" ):
         oops( "Could not find the Sdl framework." )

   elif Sdl  and  not os.path.isfile( Sdl + "/include/SDL/SDL.h" ):
      oops( "Could not find:  " + Sdl + "/include/SDL/SDL.h" )

   elif Sdl == ""  and  Platform == "linux"                                \
                   and  os.path.isfile( "/usr/include/SDL/SDL.h" ):

      Sdl = "/usr"

   elif Sdl == ""  and  not findFile( Include, "SDL/SDL.h" ):
      oops( "Could not locate the Sdl headers." )


   if Platform == "WIN32":
      if Sdl  and  not os.path.isfile( Sdl + "/lib/SDL.lib" ):
         oops( "Could not find:  " + Sdl + "/lib/SDL.lib" )

      elif Sdl == ""  and  not findFile( Library, "SDL.lib" ):
         oops( "Could not locate the Sdl libraries." )

   elif Platform == "linux":
      if Sdl  and  not os.path.isfile( Sdl + "/lib/x86_64-linux-gnu/libSDL.a" )             \
              and  not os.path.isfile( Sdl + "/lib/x86_64-linux-gnu/libSDL.so" ):

         oops( "Could not find either library:\n   "                       \
             + Sdl + "/lib/x86_64-linux-gnu/libSDL.a\n   " + Sdl + "/lib/x86_64-linux-gnu/libSDL.so\n" )

      elif Sdl == ""   and  not findFile( Library, "libSDL.a" )        \
                       and  not findFile( Library, "libSDL.so" ):

         oops( "Could not locate the Sdl libraries." )


   return



################################################################################
#
def  checkGlut():  # Locate Glut.

   global  Glut
   global  Platform
   global  Library
   global  Include
#
#...............................................................................


   if Glut:
      if not os.path.isdir( Glut ):
         oops( "Invalid Glut directory:  " + Glut )

   elif Platform == "WIN32":
      if os.path.isdir( G3D_CIT_WIN32 ):
         Glut = GLUT_CIT_WIN32


   if Platform == "OSX":
      if Glut:
         oops( "Glut needs to be installed as a framework." )

      elif not os.path.isfile( "/System/Library/Frameworks/GLUT.framework/Headers/glut.h" ):
         oops( "Could not find the Glut framework." )

   elif Platform == "linux":
      if Glut  and  not os.path.isfile( Glut + "/include/GL/glut.h" ):
         oops( "Could not find:  " + Glut + "/include/GL/glut.h" )

      elif Glut == ""  and  os.path.isfile( Glut + "/usr/include/GL/glut.h" ):
         Glut = "/usr"

      elif Glut == ""  and  not findFile( Include, "GL/glut.h" ):
         oops( "Could not locate the Glut headers." )


      if Glut  and  not os.path.isfile( Glut + "/lib/x86_64-linux-gnu/libGLU.a" )           \
               and  not os.path.isfile( Glut + "/lib/x86_64-linux-gnu/libGLU.so" ):

         oops( "Could not find either library:\n   "                       \
             + Glut + "/lib/x86_64-linux-gnu/libGLU.a\n   " + Glut + "/lib/x86_64-linux-gnu/libGLU.so" )

      elif Glut == ""  and  not findFile( Library, "libGLU.a" )            \
                       and  not findFile( Library, "libGLU.so" ):

         oops( "Could not locate the Glut libraries." )

   elif Platform == "WIN32":
      if Glut  and  not os.path.isfile( Glut + "/GL/glut.h" ):
         oops( "Could not find:  " + Glut + "/GL/glut.h" )

      elif Glut == ""  and  not findFile( Include, "GL/glut.h" ):
         oops( "Could not locate the Glut headers." )


      if Glut  and  not os.path.isfile( Glut + "/glut32.dll" ):
         oops( "Could not find:  " + Glut + "glut32.dll" )

      elif Glut == ""  and  not findFile( Library, "glut32.dll" ):
         oops( "Could not locate the Glut libraries." )


   return



################################################################################
#
def  checkVrpn():  # Locate vrpn/{include/  lib/   bin/}

   global  Vrpn
   global  Platform
   global  Word
   global  Library
   global  Include
#
#...............................................................................


   if Vrpn:
      if not os.path.isdir( Vrpn ):
         oops( "Invalid Vrpn directory:  " + Vrpn )

   elif Platform == "linux":
      if Word == 64:
         if os.path.isdir( VRPN_CIT_LINUX64 ):
            Vrpn = VRPN_CIT_LINUX64

      elif os.path.isdir( VRPN_CIT_LINUX ):
         Vrpn = VRPN_CIT_LINUX

   elif Platform == "WIN32":
      if os.path.isdir( VRPN_CIT_WIN32 ):
         Vrpn = VRPN_CIT_WIN32

   elif Platform == "OSX":
      if os.path.isdir( VRPN_CIT_OSX ):
         Vrpn = VRPN_CIT_OSX


   if Vrpn == "":
      if not findFile( Include, "vrpn_Configure.h" ):
         oops( "Could not locate the vrpn headers." )

   elif not os.path.isfile( Vrpn + "/include/vrpn_Configure.h" ):
      oops( "Could not find:  " + Vrpn + "/include/vrpn_Configure.h" )


   if Vrpn == "":
      if not findFile( Include, "quat.h" ):
         oops( "Could not locate the quat headers." )

   elif not os.path.isfile( Vrpn + "/include/quat.h" ):
      oops( "Could not find:  " + Vrpn + "/include/quat.h" )


   if Platform == "WIN32":
      if Vrpn  and  not os.path.isfile( Vrpn + "/lib/vrpn.lib" ):
         oops( "Could not find:  " + Vrpn + "/lib/vrpn.lib" )

      elif Vrpn == ""  and  not findFile( Library, "vrpn.lib" ):
         oops( "Could not locate the Vrpn libraries." )


      if Vrpn  and  not os.path.isfile( Vrpn + "/lib/quat.lib" ):
         oops( "Could not find:  " + Vrpn + "/lib/quat.lib" )

      elif Vrpn == ""  and  not findFile( Library, "quat.lib" ):
         oops( "Could not locate the Quat libraries." )

   elif Vrpn  and  not os.path.isfile( Vrpn + "/lib/libvrpn.a" )       \
              and  not os.path.isfile( Vrpn + "/lib/libvrpn.so" ):

      oops( "Could not find either library:\n   "                      \
          + Vrpn + "/lib/libvrpn.a\n   " + Vrpn + "/lib/libvrpn.so" )

   elif Vrpn == ""  and  not findFile( Library, "libvrpn.a" )           \
                    and  not findFile( Library, "libvrpn.so" ):

      oops( "Could not locate the Vrpn libraries." )

   elif Vrpn  and  not os.path.isfile( Vrpn + "/lib/libquat.a" )       \
              and  not os.path.isfile( Vrpn + "/lib/libquat.so" ):

      oops( "Could not find either library:\n   "                      \
          + Vrpn + "/lib/libquat.a\n   " + Vrpn + "/lib/libquat.so" )

   elif Vrpn == ""  and  not findFile( Library, "libquat.a" )           \
                    and  not findFile( Library, "libquat.so" ):

      oops( "Could not locate the Quat libraries." )



   return



################################################################################
#
def  checkG3d():  # Locate g3d/{include/G3D/  include/GLG3D/  lib/  bin/}

   global  G3d
   global  Platform
   global  Word            # Exit:  0 unless default Brown and 64 bit.
   global  Library
   global  Include
#
#...............................................................................


   if G3d:
      if not os.path.isdir( G3d ):
         oops( "Invalid G3d directory:  " + G3d )

   elif Platform == "linux":
      if Word == 64:
         if os.path.isdir( G3D_CIT_LINUX64 ):
            G3d = G3D_CIT_LINUX64

      elif os.path.isdir( G3D_CIT_LINUX ):
         G3d = G3D_CIT_LINUX

   elif Platform == "WIN32":
      if os.path.isdir( G3D_CIT_WIN32 ):
         G3d = G3D_CIT_WIN32

   elif Platform == "OSX":
      if os.path.isdir( G3D_CIT_OSX ):
         G3d = G3D_CIT_OSX


   # Only at Brown for the public install are lib64 and bin64 directories used.
   # This allows 32 and 64 bit installs to coexist.
   # Currently only Linux is supported.
   #
   if G3d != G3D_CIT_LINUX64:
      Word = 0


   if G3d == "":
      if not findFile( Include, "G3D/G3DAll.h" ):
         oops( "Could not locate the g3d headers." )

   elif not os.path.isfile( G3d + "/include/G3D/G3DAll.h" ):
      oops( "Could not find:  " + G3d + "/include/G3D/G3DAll.h" )


   if Platform == "WIN32":
      if G3d  and  not os.path.isfile( G3d + "/lib/G3D.lib" ):
         oops( "Could not find:  " + G3d + "/lib/G3D.lib" )

      elif G3d == ""  and  not findFile( Library, "G3D.lib" ):
         oops( "Could not locate the g3d libraries." )


   elif G3d  and  not os.path.isfile( G3d + "/lib/libG3D.a" )    \
             and  not os.path.isfile( G3d + "/lib/libG3D.so" ):

      oops( "Could not find either library:\n   "             \
          + G3d + "/lib/libG3D.a\n   " + G3d + "/lib/libG3D.so\n" )

   elif G3d == ""  and  not findFile( Library, "libG3D.a" )       \
                   and  not findFile( Library, "libG3D.so" ):

      oops( "Could not locate the g3d libraries." )


   return





################################################################################
#
def  checkMake():  # Locate the Make utilities.

   global  Make
   global  Platform
#
#...............................................................................


   if Platform == "WIN32":
      if Make == "":
         Make = MAKE_CIT_WIN32

         if not os.path.isdir( Make ):
            Make = forwardPath( "../" + MAKE_VERSION )


      if not os.path.isdir( Make ):
         oops( "Could not find the Make utilities." )

   elif Make:
      oops( "The Make utilities are only needed on Windows." )


   return



################################################################################
#
def  checkUse():  # Validate the optional component names.

   global  Use
#
#...............................................................................


   for value  in  Use.split():
      if not value in ["isense", "spacenav", "trackd", "tuio", "glut"]:
         oops( "Unknown -use option:  " + value )


   return



################################################################################
#
def  checkArgumentList():  # Validate the configutration.

   global  Sdl
   global  Glut
   global  Vrpn
   global  G3d
   global  Make
   global  Use
   global  Vrg3d
   global  Compiler
   global  Library
   global  Include
   global  Platform
#
#...............................................................................


   checkSdl()
   checkGlut()
   checkVrpn()
   checkG3d()
   checkMake()
   checkUse()

   # Locate the Vrg3d install directory.
   #
   if Vrg3d == "":
      if Platform == "linux":
         if os.path.isdir( VRG3D_CIT_LINUX ):
            Vrg3d = VRG3D_CIT_LINUX

      elif Platform == "WIN32":
         if os.path.isdir( VRG3D_CIT_WIN32 ):
            Vrg3d = VRG3D_CIT_WIN32

      elif Platform == "OSX":
         if os.path.isdir( VRG3D_CIT_WIN32 ):
            Vrg3d = VRG3D_CIT_OSX


      if Vrg3d == "":
         Vrg3d = "../install"
 


   # Find the C++ compiler.
   #
   if Compiler == "":
      if Platform == "WIN32":
         program = os.getenv( "ProgramFiles" )

         if program == None:
            oops( "Could not get the ProgramFiles environment variable." )

         elif program:
            Compiler = program + "/Microsoft Visual Studio 9.0/VC/bin/cl.exe"

         else:
            Compiler = "cl.exe"


      else:
         Compiler = "g++"


   Compiler = whichPath( Compiler )

   if Compiler == None:
      Compiler = ""

   elif Compiler:
      Compiler = forwardPath( Compiler )


   if Compiler == "":
      program = os.getenv( "ProgramFiles(x86)" )

      if program != None  and   program:
         Compiler = program + "/Microsoft Visual Studio 9.0/VC/bin/cl.exe"
         Compiler = forwardPath( whichPath( Compiler ))
 
   if Compiler == "":
      oops( "Could not find a C++ compiler." )


   printout( " G3d        " + G3d )
   printout( " Vrpn       " + Vrpn )
   printout( " Sdl        " + Sdl )
   printout( " Glut       " + Glut )

   if Library:
      printout( " Library   ", end=' ' )

      for  path in  Library:
         printout(path, end=' ' )


      printout()


   if Include:
      printout( " Include   ", end=' ' )

      for  path in  Include:
         printout( path, end=' ' )


      printout()


   if Make:
      printout( " Make       " + Make )


   printout( " Vrg3d      " + Vrg3d )

   if Use:
      printout( " Use        " + Use )


   printout( " Compiler   " + Compiler )

   return



################################################################################
#
# lifted from:  http://snippets.dzone.com/posts/show/6313
#
def whichPath( executable, path = None ):

   """Try to find 'executable' in the directories listed in 'path' (a
   string listing directories separated by 'os.pathsep'; defaults to
   os.environ['PATH']).  Returns the complete filename or None if not
   found
   """
#
#...............................................................................


   result = None

   if path is None:
      path = os.environ[ 'PATH' ]

   paths = path.split( os.pathsep )
   extlist = ['']

   if os.name == 'os2':
      (base, ext) = os.path.splitext( executable )

      # executable files on OS/2 can have an arbitrary extension, but
      # .exe is automatically appended if no dot is present in the name
      #
      if not ext:
         executable += ".exe"

   elif sys.platform == 'win32':
      pathext = os.environ['PATHEXT'].lower().split(os.pathsep)
      (base, ext) = os.path.splitext(executable)

      if ext.lower() not in pathext:
         extlist = pathext

   for ext in extlist:
      execname = executable + ext

      if os.path.isfile(execname):
         result = execname
         break

      else:
         for p in paths:
            f = os.path.join(p, execname)

            if os.path.isfile(f):
               result = f
               break

   #else:
   #    return None

   if result:
      result = os.path.realpath( os.path.abspath( result ))


   return  result



################################################################################
#
def forwardPath( path ):  # Absolute path with forward slashes and
#                           no trailing slash.
#
#...............................................................................


   return os.path.abspath( path ).replace( "\\", "/" ).rstrip( "/" )



################################################################################
#
def writeConfigure( configure = "configure.make" ):

   global  Platform
#
#...............................................................................


   status = 0

   try:  id = open( configure, 'w')
   except IOError:
      reportFault( "Could not open the configuration file for writing." \
                   "open( " + configure + ", 'w') failed." )

      status = -1


   if status == 0:
      status = writeParameter( id )

      if status == 0:
         if Platform == "linux":
            status = writeLinux( id )

         elif Platform == "WIN32":
            status = writeWindows( id )

         elif Platform == "OSX":
            status = writeMac( id )

 
      if status == 0:
         id.write( "\n" )
         id.write( "\n" )
         id.write( "# Device specific configuration settings.\n" )
         id.write( "#\n" )
         id.write( "include  $(VRG3D_SOURCE)/vrg3d/device.make\n" )
         id.write( "\n" )
         id.write( "#\n" )
         id.write( "################################################################################\n" )
 
 
      id.close()


   return  status



################################################################################
#
def writeParameter( id ):

   global  Sdl
   global  Vrpn
   global  G3d
   global  Glut
   global  Make
   global  Vrg3d
   global  Compiler
   global  Library
   global  Include
   global  Platform
   global  Word
#
#...............................................................................


   status = 0

   try:
      id.write( "################################################################################\n" )
      id.write( "#\n" )
      id.write( "#  This file was generated by configure.py on:  " +            \
                      time.asctime( time.localtime( time.time() )) + "\n" )
      id.write( "#\n" )
      id.write( "#...............................................................................\n" )

      id.write( "\n" )
      id.write( "\n" )

      id.write( "########################  configuration parameters  ############################\n" )
      id.write( "#\n" )
      id.write( "PLATFORM = " + Platform + "\n" )
      id.write( "COMPILER = " + forwardPath( Compiler ) + "\n" )
      id.write( "\n" )

      if G3d:
         id.write( "IMPORT_G3D  = " + forwardPath( G3d ) + "\n" )


      if Vrpn:
         id.write( "IMPORT_VRPN = " + forwardPath( Vrpn ) + "\n" )


      if Sdl:
         id.write( "IMPORT_SDL  = " + forwardPath( Sdl ) + "\n" )


      if Glut:
         id.write( "IMPORT_GLUT = " + forwardPath( Glut ) + "\n" )


      if Make:
         id.write( "IMPORT_MAKE = " + forwardPath( Make ) + "\n" )


      id.write( "\n" )
      id.write( "INSTALL_VRG3D = " + forwardPath( Vrg3d ) + "\n" )

      if Use:
         id.write( "\n" )

         for value  in  Use.split():
            pad = ""

            for i  in  range( 9 - len( value )):
                pad += " "

            id.write( "USE_" + value.upper() + pad + " = 1" + "\n" )


      if Word:
         id.write( "WORD          = " + str( Word ) + "\n" )

      id.write( "#\n" )
      id.write( "#...............................................................................\n" )

   except os.error:
      printout( "ERROR:  Could not write to the configuration file." )
      status = -1


   return  status



################################################################################
#
def writeLinux( id ):

   global  Sdl
   global  Vrpn
   global  G3d
   global  Glut
   global  Make
   global  Use
   global  Vrg3d
   global  Compiler
   global  Library
   global  Include
#
#...............................................................................


   status = 0

   try:
      id.write( "\n" )
      id.write( "\n" )

      id.write( "#############################  Compiler options  ###############################\n" )
      id.write( "#\n" )
      id.write( "VRG3D_SOURCE := " + forwardPath( os.getcwd() + "/.." ) + "\n" )
      id.write( "\n" )

      id.write( 'CXXINC   += -I$(VRG3D_SOURCE)\n' )

      if G3d  and  G3d != "/usr":
         id.write( 'CXXINC   += -I$(IMPORT_G3D)/include\n' )


      if Vrpn  and  Vrpn != "/usr":
         id.write( 'CXXINC   += -I$(IMPORT_VRPN)/include\n' )


      if Sdl  and  Sdl != "/usr":
         id.write( 'CXXINC   += -I$(IMPORT_SDL)\n' )


      if Glut  and  Glut != "/usr":
         id.write( 'CXXINC   += -I$(IMPORT_GLUT)\n' )


      for  path  in  Include:
         id.write( "CXXINC   += -I" + forwardPath( path ) + "\n" )


      id.write( "\n" )
      id.write( "CXXFLAGS   += -DUSE_VRPN  -Wno-deprecated\n" )
      id.write( "DEBUG_FLAGS = -DDEBUG=1\n" )
      id.write( "#\n" )
      id.write( "#...............................................................................\n" )

      id.write( "\n" )
      id.write( "\n" )

      id.write( "##############################  Linker options  ################################\n" )
      id.write( "#\n" )
      id.write( 'LDLIB   += -L$(VRG3D_SOURCE)/vrg3d/obj\n' )
      id.write( "LDLIB_O += -lvrg3d\n" )
      id.write( "LDLIB_G += -lvrg3dd\n" )
      id.write( "\n" )

      if G3d  and  G3d != "/usr":
         id.write( 'LDLIB   += -L$(IMPORT_G3D)/lib\n' )

      id.write( "LDLIB_O += -lGLG3D     -lG3D\n" )
      id.write( "LDLIB_G += -lGLG3Dd    -lG3Dd\n" )
      #id.write( "LDLIB_G += -lGLG3D     -lG3D\n" )
      id.write( "LDLIB   += -lGL        -lGLU\n" )
      id.write( "LDLIB   += -lavformat  -lavcodec  -lavutil\n" )
      id.write( "LDLIB   += -lz         -lzip      -lpng     -ljpeg\n" )
      id.write( "\n" )

      if Vrpn  and  Vrpn != "/usr":
         id.write( 'LDLIB   += -L$(IMPORT_VRPN)/lib\n' )


      id.write( "LDLIB   += -lquat  -lvrpn\n" )
      id.write( "\n" )

      if Sdl  and  Sdl != "/usr":
         id.write( 'LDLIB   += -L$(IMPORT_SDL)\n' )


      id.write( "LDLIB   += -lSDL   -lSDLmain\n" )

      for path  in  Library:
         id.write( "LDLIB   += -L" + forwardPath( path ) + "\n" )


      if "glut"  in  Use.split():
         id.write( "LDLIB   += -lglut\n" )


      id.write( "#\n" )
      id.write( "#...............................................................................\n" )

   except os.error:
      printout( "ERROR:  Could not write to the configuration file." )
      status = -1

   return  status



################################################################################
#
def writeMac( id ):

   global  Sdl
   global  Vrpn
   global  G3d
   global  Glut
   global  Make
   global  Vrg3d
   global  Compiler
   global  Library
   global  Include
#
#...............................................................................


   status = 0

   try:
      id.write( "\n" )
      id.write( "\n" )

      id.write( "#############################  Compiler options  ###############################\n" )
      id.write( "#\n" )
      id.write( "VRG3D_SOURCE := " + forwardPath( os.getcwd() + "/.." ) + "\n" )
      id.write( "\n" )

      id.write( 'CXXINC   += -I$(VRG3D_SOURCE)\n' )

      if G3d:
         id.write( 'CXXINC   += -I$(IMPORT_G3D)/include\n' )


      if Vrpn:
         id.write( 'CXXINC   += -I$(IMPORT_VRPN)/include\n' )


      ###id.write( 'CXXINC   += -I$(IMPORT_SDL)\n' )

      for  path  in  Include:
         id.write( "CXXINC   += -I" + forwardPath( path ) + "\n" )

      id.write( "\n" )

      id.write( "CXXFLAGS   += -DUSE_VRPN  -Wno-deprecated\n" )
      id.write( "DEBUG_FLAGS = -DDEBUG=1\n" )

      id.write( "#\n" )
      id.write( "#...............................................................................\n" )

      id.write( "\n" )
      id.write( "\n" )

      id.write( "##############################  Linker options  ################################\n" )
      id.write( "#\n" )
      id.write( 'LDLIB   += -L$(VRG3D_SOURCE)/vrg3d/obj\n' )
      id.write( "LDLIB_O += -lvrg3d\n" )
      id.write( "LDLIB_G += -lvrg3dd\n" )
      id.write( "\n" )

      if G3d  and  G3d != "/usr":
         id.write( 'LDLIB   += -L$(IMPORT_G3D)/lib\n' )

      id.write( "LDLIB_O += -lGLG3D     -lG3D\n" )
      #id.write( "LDLIB_G += -lGLG3Dd    -lG3Dd\n" )
      id.write( "LDLIB_G += -lGLG3D     -lG3D\n" )
      #id.write( "LDLIB   += -lGL        -lGLU\n" )
      id.write( "\n" )

      if Vrpn  and  Vrpn != "/usr":
         id.write( 'LDLIB   += -L$(IMPORT_VRPN)/lib\n' )


      id.write( "LDLIB   += -lquat  -lvrpn\n" )
      id.write( "\n" )

      id.write( "LDLIB   += -framework  Carbon  -framework  AGL\n" )
      id.write( "LDLIB   += -framework  GLUT    -framework  OpenGL\n" )
      id.write( "LDLIB   += -framework  SDL\n" )

      id.write( "LDLIB   += -lavformat  -lavcodec  -lavutil\n" )
      id.write( "LDLIB   += -lz         -lzip      -lpng     -ljpeg\n" )

      ###if Sdl != "/usr":
      ###   id.write( 'LDLIB   += -L$(IMPORT_SDL)/lib\n' )
      ###id.write( "LDLIB   += -lSDL   -lSDLmain\n" )

      for path  in  Library:
         id.write( "LDLIB   += -L" + forwardPath( path ) + "\n" )


      id.write( "#\n" )
      id.write( "#...............................................................................\n" )

   except os.error:
      printout( "ERROR:  Could not write to the configuration file." )
      status = -1

   return  status



################################################################################
#
def writeWindows( id ):

   global  Sdl
   global  Vrpn
   global  G3d
   global  Glut
   global  Make
   global  Vrg3d
   global  Compiler
   global  Library
   global  Include
#
#...............................................................................


   status = 0

   try:
      id.write( "\n" )
      id.write( "\n" )
      id.write( "############################  Windows commands  ################################\n" )
      id.write( "#\n" )

      id.write( "MAKE   = $(IMPORT_MAKE)/make.exe\n" )
      id.write( "MKDIR  = $(IMPORT_MAKE)/mkdir.exe\n" )
      id.write( "RM     = $(IMPORT_MAKE)/rm.exe\n" )
      id.write( "CP     = $(IMPORT_MAKE)/cp.exe\n" )

      id.write( "#\n" )
      id.write( "#...............................................................................\n" )

      id.write( "\n" )
      id.write( "\n" )

      id.write( "#############################  Compiler options  ###############################\n" )
      id.write( "#\n" )
      id.write( "VRG3D_SOURCE := " + forwardPath( os.getcwd() + "/.." ) + "\n" )
      id.write( "\n" )

      id.write( 'CXXINC   += -I"$(VRG3D_SOURCE)"\n' )

      if G3d:
         id.write( 'CXXINC   += -I"$(IMPORT_G3D)/include"\n' )


      if Vrpn:
         id.write( 'CXXINC   += -I"$(IMPORT_VRPN)/include"\n' )


      if Sdl:
         id.write( 'CXXINC   += -I"$(IMPORT_SDL)/include"\n' )


      if Glut:
         id.write( 'CXXINC   += -I"$(IMPORT_GLUT)/include"\n' )


      for  path  in  Include:
         id.write( 'CXXINC   += -I"' + forwardPath( path ) + '"\n' )

      id.write( "\n" )

      id.write( "CXXFLAGS   += -nologo  -D__i386__   -EHsc  -DUSE_VRPN\n" )
      id.write( "DEBUG_FLAGS = -DDEBUG=1\n" )
      id.write( "#\n" )
      id.write( "#...............................................................................\n" )

      id.write( "\n" )
      id.write( "\n" )

      id.write( "##############################  Linker options  ################################\n" )
      id.write( "#\n" )
      id.write( 'LDLIB   += -LIBPATH:"$(VRG3D_SOURCE)/vrg3d/obj"\n' )
      id.write( "LDLIB_O += vrg3d.lib\n" )
      id.write( "LDLIB_G += vrg3dd.lib\n" )
      id.write( "\n" )

      id.write( "LDLIB   += -LIBPATH:../obj\n" )
      id.write( "\n" )

      if G3d:
         id.write( 'LDLIB   += -LIBPATH:"$(IMPORT_G3D)/lib"\n' )


      id.write( "LDLIB_O += GLG3D.lib     G3D.lib\n" )
      id.write( "LDLIB_G += GLG3Dd.lib    G3Dd.lib\n" )
      #id.write( "LDLIB_G += GLG3D.lib     G3D.lib\n" )
      id.write( "LDLIB   += avformat.lib  avcodec.lib  avutil.lib\n" )
      id.write( "LDLIB   += zlib.lib    zip.lib    png.lib   jpeg.lib\n" )
      id.write( "\n" )

      if Vrpn:
         id.write( 'LDLIB   += -LIBPATH:"$(IMPORT_VRPN)/lib"\n' )


      id.write( "LDLIB   += quat.lib  vrpn.lib\n" )
      id.write( "\n" )

      if Sdl:
         id.write( 'LDLIB   += -LIBPATH:"$(IMPORT_SDL)"\n' )


      id.write( "LDLIB   += SDL.lib  SDLmain.lib\n" )

      for path  in  Library:
         id.write( 'LDLIB   += -LIBPATH:"' + forwardPath( path ) + '"\n' )


      if "glut"  in  Use.split():
         id.write( 'LDLIB   += -LIBPATH:"$(IMPORT_GLUT)"\n' )

      id.write( "#\n" )
      id.write( "#...............................................................................\n" )

   except os.error:
      printout( "ERROR:  Could not write to the configuration file." )
      status = -1

   return  status



################################################################################
################################################################################
################################################################################
################################################################################
################################################################################


status   = 0
Platform = platform.system().lower()

if Platform == "windows"  or  Platform == "microsoft":
   Platform = "WIN32"

elif Platform == "darwin":
   Platform = "OSX"

elif Platform != "linux":
   oops( "ERROR:  Unsupported platform:  " + Platform )


Word = 32

if platform.architecture()[0] == "64bit":
   Word = 64


if status == 0:
   status = setArgumentList()


if status == 0:
   checkArgumentList()
   status = writeConfigure()


if status:
   printout( "*********  Exiting with failed status = " + status + " *********" )

else:
   printout( "*********  Sucessfully created:  configure.make  *********" )
   printout( "   You can now run:" )
   printout( "      make  allclean     # Remove prior build results." )
   printout( "      make  all          # Build vrg3d, demo, and server." )
   printout( "      make  install      # Install vrg3d, demo, and server." )


sys.exit( status )

#
#######################  end  vrg3d/vrg3d/configure.py  ########################
