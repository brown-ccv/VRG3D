#######  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


##########################  common/vrg3d/osx.make  #############################
#
#  OSX Make targets for Vrg3d.
#
#...............................................................................


# Compile and link using the C++ compiler.  Note defaults are set by Gnu make.
#
CXX := $(COMPILER)
LD  := $(COMPILER)


CXXFLAGS += -arch i386
LDLIB    += -arch i386  -lc  -lm  -ldl

ifdef PROGRAM
PROGO := $(PROGRAM)
PROGG := $(PROGRAM)d

default:  obj/$(PROGO)

debug:    obj/$(PROGG)
endif


ifdef LIBRARY
LIBO := lib$(LIBRARY).a
LIBG := lib$(LIBRARY)d.a

default:  obj/$(LIBO)

debug:    obj/$(LIBG)
endif


OBJ   := $(CXXSOURCE:.cpp=.o)
OBJO  := $(OBJ:.o=.oo)
OBJG  := $(OBJ:.o=.og)

BOBJO := $(OBJ:%.o=obj/%.oo)
BOBJG := $(OBJ:%.o=obj/%.og)



# Compile a file.
#
obj/%.og: %.cpp
	$(CXX) $< -c  -o obj/$*.og  $(CXXFLAGS)  \
          $(CXXINC)                         \
          -g   $(DEBUG_FLAGS)



obj/%.oo: %.cpp
	$(CXX) $< -c  -o obj/$*.oo  $(CXXFLAGS)  \
          $(CXXINC)



# Create a library.
#
ifdef LIBO
obj/$(LIBO):  mkdir_obj  $(BOBJO)
	@$(ECHO)  "   <<< Archive:  obj/$(LIBO) >>>"
	cd  obj  &&  $(AR) -rv $(LIBO) $(OBJO)
endif

ifdef LIBG
obj/$(LIBG):  mkdir_obj  $(BOBJG)
	@$(ECHO)  "   <<< Archive:  obj/$(LIBG) >>>"
	cd  obj  &&  $(AR) -rv $(LIBG) $(OBJG)
endif



# Link a program
#
ifdef PROGO
obj/$(PROGO):   mkdir_obj  $(BOBJO)  $(BOBJO)
	@$(ECHO)  "   <<< Link:  $(PROGO) >>>"
	$(LD)  -o obj/$(PROGO)  $(BOBJO)  \
           $(LDLIB_O) $(LDLIB)
endif


ifdef PROGG
obj/$(PROGG):   mkdir_obj  $($(PROG)BOBJG)  $(BOBJG)
	@$(ECHO)  "   <<< Link:  $(PROGG) >>>"
	$(LD)  -o obj/$(PROGG)  $($(PROG)BOBJG) \
           $(LDLIB_G)  $(LDLIB)
endif


#
#######################  end  common/vrg3d/osx.make  ###########################
