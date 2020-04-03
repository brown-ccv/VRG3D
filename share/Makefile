#######  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

###########################  vrg3d/share/Makefile  #############################
#
#   Install fonts and setup files.
#...............................................................................
#
#  make  install    Install fonts and setup files.
#...............................................................................


.PHONY:  default
default:  all


include  ../vrg3d/configure.make
include  ../vrg3d/common.make


SHARE_FILE = arial.fnt       console-small.fnt   eurostyle-small.fnt  \
             eurostyle.fnt   greek.fnt           icon.fnt             \
             osx.skn         twilight.skn


.PHONY: all
all:
	@echo  "No binaries to make."


.PHONY: clean
clean:
	@echo  "Nothing to clean."


.PHONY: allclean
allclean:  clean


.PHONY:  install
install:
	$(MKDIR) -p  $(INSTALL_VRG3D)/share/vrg3d
	$(CP)    -f  $(SHARE_FILE)  $(INSTALL_VRG3D)/share/vrg3d
	$(MAKE)  -C  vrsetup  install


#
#########################  end  vrg3d/share/Makefile  ##########################
