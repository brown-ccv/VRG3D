Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


        -----------------  vrg3d/READ_ME.txt  -----------------


                    Vrg3d for Windows, OSX and Linux

     Version pre.5                                  March 15, 2011

This is a pre-release of Vrg3d intended only for Vrg3d collaborators.
It is being used as a starting point to transition to an initial stable
release.  The API is expected to change and is not be used by end users
until a stable release is available.

The project has not been installed or used outside the Brown programming
environment.  Vrg3d has only been lightly tested within the Brown environment.
Any software that has not been tested in a foreign environment can not be
expected to work out of the box.  You should expect it to take several
interactive iterations before the software can be installed and used in
your environment.

      ====================================================================

                       Installing Vrg3d

The files INSTALL_WINDOWS.log and INSTALL_LINUX.log contain transcripts of
example successful installations for Windows and Linux.


1. Windows-only:  Set up build utilities.

      The make files are written for Visual Studio 2008 (a.k.a version 9).
      You can download Visual Studio 2008 Express for free here:

         http://www.microsoft.com/express/Downloads/


      Add the Unix utilities for Windows to the PATH environment variable.
      They are in the utility_windows\ included in the vrg3d distribution
      and can be copied to any convenient directory.  Detailed installation
      instructions are in the import\make\utility_windows\READ_ME.txt file.
      Check to see that you can run the commands utility_windows\*.exe
      from the command line.

      If you have these utilities installed somewhere you can designate
      the location on the configure.py script with the "-make" option.
      Otherwise the script will use the copy in the source distribution.


2. VRG3D depends on SDL, Glut, G3D-8.01, and vrpn-7.26.

   For linux, SDL and Glut will probably be located automatically by
   running the VRG3D 'configure.py' script, but G3D and vrpn may need to
   be built if they are not generally available on your system.

   See the appendix below for more details on locating or building
   these libraries.


3. Build and install vrg3d

   Place the vrg3d source code in a build directory and go into the
   vrg3d/ subdirectory containing the library source code.  First
   determine the paths you will use to run the configure.py script.
   It creates the file configure.make; which is subsequently included
   by the Make files.  The options to configure.py are:

       -g3d      <path>  # Location of g3d
       -vrpn     <path>  # Location of vrpn
       -sdl      <path>  # Location of sdl (Windows and Linux only)
       -glut     <path>  # Location of glut (Windows and Linux only)
       -library  <path>  # Additional library directories
       -include  <path>  # Additional include directories
       -make     <path>  # Location of the Make utilities (Windows only)
       -vrg3d    <path>  # Location to install vrg3d
       -compiler <name>  # Name or path of the C++ compiler


   Not all of these options are required depending on the platform and
   whether an installed package can be located on your system.  You can
   safely run the configure.py script repeatedly in case you don't get
   the options right the first time.  The script will list the paths
   used and you can also examine the configure.make file it generates.

      > python  configure.py  [options]
      > make  allclean     # Remove prior build results.
      > make  all          # Build vrg3d, demo, and server.
      > make  install      # Install vrg3d, demo, and server.

   You can also build each component independently by running the
   make command in each directory with no target.



APPENDIX:

The following sections help check whether VRG3D dependencies are
installed on your system.  If they are not, information is given on
how to obtain and build the dependencies.


1. Set up g3d 8.01

   Check with your system administrator to see if G3D 8.01 is installed
   on your system.  If not download g3d 8.01 from:

       http://g3d.sourceforge.net/downloads.html


   * Windows:  If you do not want to build g3d from source you can use the
                g3d binary distribution:

     http://downloads.sourceforge.net/g3d/G3D-8.01-win32.zip


   * Linux:  You will need to build the g3d source distribution:

         http://downloads.sourceforge.net/g3d/G3D-8.01-src.zip


       Follow the instructions on the g3d site.


   * OSX:  Install G3D binaries from:

       http://downloads.sourceforge.net/g3d/G3D-8.01-osx.zip

       Follow the instructions on the g3d site.


2. Set up Vrpn

   Download Vprn from:  ftp://ftp.cs.unc.edu/pub/packages/GRIP/vrpn/

   Vrpn has multiple build mechanisms and they change between versions.
   You may have to adjust the Vrpn build process and move some files
   around.  You should set up an install directory contining:

      include/    quat.h   vrpn_*.h

      lib/        quat.lib   quatd.lib   quatd.pdb
                  vrpn.lib   vrpnd.lib   vrpnd.pdb


3. Check to see that Sdl is installed.

   * Linux:  Sdl is usually installed in:  /usr/include/SDL/*.h
                                           /usr/lib/libSDL*

       If you can't find it check with your system administrator.


   * Windows:   Download the Sdl Development Library from:

                       http://www.libsdl.org/


       To install Sdl on Windows get a recent kit:   SDL-1.2.14-VC8.zip
       Unzip it to an install directory.

       Add the Sdl lib/ directory path to the PATH environment variable.

       Note that even though the Windows distribution of Sdl is compiled
       with cl8 it will still link with cl9.  Some, if not all versions
       are not organized like the Linux release.  You may need to move
       some directories around to look like this:

           include/SDL/   SDL.h  SDL_*.h  begin_code.h  close_code.h

           lib/           SDL.dll      SDL.lib      SDLmain.lib


   * OSX:  Download sdl from:    http://www.libsdl.org/download-1.2.php

       Install the SDL framework package in the /Library/Frameworks
       directory by clicking on the "SDL-1.2.14.dmg" link for:

           http://www.libsdl.org/release/SDL-1.2.14.dmg
       

4. Check to see that Glut is installed.

   * Linux:  Glut is usually installed in:  /usr/include/GL/*.h
                                              /usr/lib/libglut*

       If you can't find it check with your system administrator.


   * Windows:  Download glut from:  http://www.xmission.com/~nate/glut.html


       The most recent kits are:   glut-3.7.6-bin.zip
                                   glut-3.7.6-src.zip


   * OSX:  The GLUT framework should already be installed as part of OSX.
       It should be installed along with OpenGl, AGL, and Carbon in the
       /System/Library/Frameworks/ directory.


      -----------------  end  vrg3d/READ_ME.txt  -----------------
