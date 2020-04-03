                      Unix Utilities for Windows

     Version 1.0                                    March 14, 2011


The directory vrg3d/utility_windows_1.0/ contains a minimum set of utilities
needed to build vrg3d on Windows.



                       INSTALLING THE UTILITIES



You can copy the vrg3d/utility_windows_1.0/ directory to any convenient
location.  Set PATH variable to the dircetory containing the utilities.

Note that if you have a buggy set of utilities installed such as Cygwin
or MinGW be sure to reference this collection before them on the PATH
environment variable.  Alternatively, you can also install the GNU utilities
in full if you would like a more complete set.




                            CONTENTS

While implementations of these programs are available from many sources, most
have bugs that prevent them from being useful.  Due to bugs this collection was
cobbled together from two sources.


The distribution contains:

    diff.exe        false.exe        mkdir.exe    mkdir_gnu.exe
    cp.exe          echo.exe         make.exe     rm.exe
    libiconv2.dll   libintl3.dll 


They were extracted from:

   http://sourceforge.net/projects/unxutils/
   UnxUtils.zip ==> mkdir.exe

      Note that this program has the following bug:

         > mkdir -p  obj/        #Bug:  Gives a warning and returns status 1.
         > mkdir -p  obj         # Ok:  No warning and the status is zero.

      The $G Makefile's have been written to work around this bug.
      You can also use mkdir_gnu.exe by renaming it to avoid this bug.
      However, mkdir_gnu.exe also has a bug in that it does not work
      when installed on a Network File System.  If you install the
      utilities on a physical drive (a.k.a. the C: drive) it will work.


   http://gnuwin32.sourceforge.net/packages/coreutils.htm
   http://sourceforge.net/projects/gnuwin32/files/coreutils/5.3.0/coreutils-5.3.0-bin.zip/download
      coreutils-5.3.0-bin.zip
      cp.exe    echo.exe    false.exe    rm.exe    mkdir.exe ==> mkdir_gnu.exe


   http://gnuwin32.sourceforge.net/packages/diffutils.htm
   http://sourceforge.net/projects/gnuwin32/files/diffutils/2.8.7-1/diffutils-2.8.7-1-bin.zip/download
      diffutils-2.8.7-1-bin.zip
      diff.exe


   http://gnuwin32.sourceforge.net/packages/make.htm
   http://sourceforge.net/projects/gnuwin32/files/make/3.81/make-3.81-bin.zip/download
      make-3.81-bin.zip
      make.exe


   http://sourceforge.net/projects/gnuwin32/files/make/3.81/make-3.81-dep.zip/download
      libiconv2.dll       Convert between character encodings.
      libintl3.dll        Library for native language support.

    ---------------------------  end  READ_ME.txt  -------------------------
