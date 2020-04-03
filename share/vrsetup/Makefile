########################  vrg3d/share/vrsetup/Makefile  ########################
#
#   Install setup files.
#...............................................................................
#
#  make  install    Install only the setup files.
#...............................................................................


.PHONY:  default
default:  all


include  ../../vrg3d/configure.make
include  ../../vrg3d/common.make


SHARE_FILE = 3DDLP.vrsetup                                \
             blueline.vrsetup                             \
             brown_tiledwall_doubletile1.vrsetup          \
             brown_tiledwall_doubletile2.vrsetup          \
             brown_tiledwall_doubletile3.vrsetup          \
             brown_tiledwall_doubletile4.vrsetup          \
             brown_tiledwall_tile5.vrsetup                \
             cave-devices-audiocave.cfg                   \
             cave-devices.cfg                             \
             cave1wall.vrsetup                            \
             cavefloor-winserv.vrsetup                    \
             cavefloor-winservmono.vrsetup                \
             cavefloor.vrsetup                            \
             cavefloormono.vrsetup                        \
             cavefront-winserv.vrsetup                    \
             cavefront-winservmono.vrsetup                \
             cavefront.vrsetup                            \
             cavefrontmono.vrsetup                        \
             caveleft-winserv.vrsetup                     \
             caveleft-winservmono.vrsetup                 \
             caveleft.vrsetup                             \
             caveleftmono.vrsetup                         \
             caveright-winserv.vrsetup                    \
             caveright-winservmono.vrsetup                \
             caveright.vrsetup                            \
             caverightmono.vrsetup                        \
             cit304-devices.cfg                           \
             cit304-with-phantom-devices.cfg              \
             depthq-client.vrsetup                        \
             depthq.vrsetup                               \
             desktop.vrsetup                              \
             desktop640x480.vrsetup                       \
             desktopfullscreen.vrsetup                    \
             dive-devices-orig.cfg                        \
             dive-devices.cfg                             \
             fishtank.vrsetup                             \
             fishtank304.vrsetup                          \
             fishtank304mono.vrsetup                      \
             fishtank304nodevices.vrsetup                 \
             fishtank304nodevicesmono.vrsetup             \
             fishtankmono.vrsetup                         \
             forceserver.vrsetup                          \
             msi-frontwallL10.vrsetup                     \
             msi-frontwallL11.vrsetup                     \
             msi-frontwallL12.vrsetup                     \
             msi-frontwallL15.vrsetup                     \
             msi-frontwallL16.vrsetup                     \
             msi-frontwallL19.vrsetup                     \
             msi-frontwallL4.vrsetup                      \
             msi-frontwallL5.vrsetup                      \
             msi-frontwallL6.vrsetup                      \
             msi-frontwallL7.vrsetup                      \
             msi-frontwallL8.vrsetup                      \
             msi-frontwallL9.vrsetup                      \
             msi-devices.cfg                              \
             msi-frontwall.vrsetup                        \
             msi-leftwall.vrsetup                         \
             msi-rightwall.vrsetup                        \
             mttablewall-devices.cfg                      \
             multitouch-client.vrsetup                    \
             multitouch-desktop.vrsetup                   \
             multitouch.vrsetup                           \
             pt-samsung.vrsetup                           \
             pt.vrsetup                                   \
             radimenzo-devices.cfg                        \
             radimenzo-samsung.vrsetup                    \
             radimenzo.vrsetup                            \
             samsung-devices.cfg                          \
             samsung-devices-optitrack.cfg                \
             samsung-devices-prop.vrsetup                 \
             samsung-testdevices.vrsetup                  \
             samsung.vrsetup                              \
             samsung-with-devices-optitrack-pt.vrsetup    \
             samsung-with-devices-optitrack.vrsetup       \
             samsung-with-devices.vrsetup                 \
             tuiodesktop.vrsetup                          \
             tuio-localhost.cfg                           \
             testclient.vrsetup


.PHONY: all
all:
	@echo  "No binaries to make."


.PHONY: clean
clean:
	@echo  "Nothing to clean."


.PHONY: allclean
allclean:  clean


#  This is explicitly done becuae the make files do not yet support
#  nested share directories.
#
.PHONY:  install
install:
	$(MKDIR)  -p  $(INSTALL_VRG3D)/share/vrg3d/vrsetup
	$(CP)     -f  $(SHARE_FILE)  $(INSTALL_VRG3D)/share/vrg3d/vrsetup


#
###################  end  vrg3d/share/vrsetup/Makefile  ########################
