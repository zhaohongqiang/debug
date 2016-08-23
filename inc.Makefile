
ifndef DEPEND_FILE
	DEPEND_FILE = Makefile.d
endif

ifndef MAKEFILE_NAME
	MAKEFILE_NAME = Makefile
endif	

ifndef LOCAL_MAKEFILE
	LOCAL_MAKEFILE = $(MAKEFILE_NAME)
endif


ifndef CCOMPILER
	CCOMPILER := gcc
endif

ifndef CXXCOMPILER
	CXXCOMPILER := g++
endif

#CROSS := /opt/toolchains/crosstools7405/bin/mipsel-linux-
CROSS := arm-none-linux-gnueabi-
CC := $(CROSS)$(CCOMPILER)
CXX := $(CROSS)$(CXXCOMPILER)

#CC := mipsel-linux-gcc

LD := $(CROSS)$(LD)
OBJCOPY := $(CROSS)objcopy
AR := $(CROSS)ar
STRIP := $(CROSS)strip

ifndef LINKER
	LINKTOOL = $(CC)   # must be = and not :=
else
	LINKTOOL := $(CROSS)$(LINKER)
endif

LOCALDIR = $(shell pwd)

ifndef RMPREFIX
	export RMPREFIX := $(LOCALDIR)/..
endif

ifeq "$(TARGET_TYPE)" "MICROCODE"
	ifndef SPASM
		ifdef SPASM_VERSION
#if SPASM version 2.x.x, better not to include the libraries
ifeq ($(findstring 2., $(SPASM_VERSION)), 2.)
                SPASM := LANG=C perl /utils/em8xxx/spasm/$(SPASM_VERSION)/bin/spasm
else
                SPASM := LANG=C perl -I /utils/em8xxx/spasm/$(SPASM_VERSION)/lib /utils/em8xxx/spasm/$(SPASM_VERSION)/bin/spasm
endif
		else
$(error Please define SPASM_VERSION (ex: 1.4.18))
		endif 
	endif

	ifndef SPBIN
		ifdef SPBIN_VERSION
		SPBIN := /utils/em8xxx/spbin/$(SPBIN_VERSION)/bin/spbin
		else
$(error Please define SPBIN_VERSION (ex: 1.2.9))
		endif
	endif
endif

APIDIR := $(RMPREFIX)/externalapi
LIBDIR := $(RMPREFIX)/lib 
DISTDIR := $(RMPREFIX)/dist

MAKEARGS = -f $(MAKEFILE_NAME) ## the -f option is not passed through MAKEFLAGS
# note: if I remember -fwritable-strings is only for havana gui code

#PREPROCESSORFLAGS += $(RMCFLAGS) -I. -D_REENTRANT -fwritable-strings
PREPROCESSORFLAGS += $(RMCFLAGS) -I. 

#########################
## Compilation options ##
#########################

SPASMFLAGS +=
SPBINFLAGS += -l -p -m -s -od 0x10000

# It has to be a = not a :=
ASFLAGS = $(PREPROCESSORFLAGS)

ifeq ($(findstring leakchecker, $(COMPILKIND)), leakchecker)
EXEFLAGS += `glib-config --libs glib`
PREPROCESSORFLAGS += -DUSE_LEAK_CHECKER=1
endif

ifeq ($(findstring withthreads, $(COMPILKIND)), withthreads)
PREPROCESSORFLAGS += -DWITH_THREADS=1
#yang LDFLAGS += -lpthread
endif

ifeq ($(findstring withfeeblemm, $(COMPILKIND)), withfeeblemm)
# when memory is full, RMMalloc returns NULL and does not RMPanic
# when RMFree is called at a bad address, does not RMPanic
# useful to stresstest the memory manager
PREPROCESSORFLAGS += -DWITH_FEEBLEMM=1
endif

ifeq ($(findstring withdl, $(COMPILKIND)), withdl)
EXEFLAGS += -ldl -rdynamic
endif

ifeq ($(findstring kernel, $(COMPILKIND)), kernel)	## kernel compilation

PREPROCESSORFLAGS += -D__KERNEL__ -DMODULE -DEXPORT_SYMTAB

ifeq ($(findstring uclinux, $(COMPILKIND)), uclinux)	## uCLinux compilation

ifndef MUM_KI
MUM_KI=-I$(UCLINUX_KERNEL)/include
endif

ifndef KERNELSTYLE
KERNELSTYLE= \
        -fomit-frame-pointer \
        -fno-strict-aliasing \
        -fno-common \
	-pipe \
	-fno-builtin
PREPROCESSORFLAGS += -D__linux__
endif

else # no uclinux

ifndef MUM_KI
MUM_KI=-I/usr/src/linux-$(shell uname -r)/include -I/usr/src/linux-2.4/include
endif

ifndef KERNELSTYLE
KERNELSTYLE= \
        -fomit-frame-pointer \
        -fno-strict-aliasing \
        -fno-common \
	-mpreferred-stack-boundary=2 \
	-pipe 
endif

endif # linux/uclinux alternative done


## Switch for fascist warnings.
ifdef DISABLE_WARNINGS
CWARNINGS+= \
	-Wno-import \
        -Wunused \
        -Wimplicit \
        -Wmain \
        -Wreturn-type \
        -Wswitch \
        -Wtrigraphs \
        -Wchar-subscripts \
        -Wparentheses \
        -Wpointer-arith \
        -Wcast-align \
	-Wuninitialized -O
else
CWARNINGS+= -O2 
endif

CFLAGS += $(KERNELSTYLE) $(MUM_KI)

else # user compilation

## Switch for fascist warnings.
#define DISABLE_WARNINGS

ifdef DISABLE_WARNINGS
COMMONWARNINGS+= \
	-Wundef \
	-Wall \
	-Wchar-subscripts \
	-Wmissing-prototypes \
	-Wsign-compare \
	-Wuninitialized -O
#	-Werror 
CWARNINGS:=$(COMMONWARNINGS) -Wnested-externs -Wmissing-declarations 
CXXWARNINGS:=$(COMMONWARNINGS) -fcheck-new 
endif

### if libc heades are needed
ifndef USE_STD_LIB
	PREPROCESSORFLAGS += -nostdinc
endif

endif # kernel/user alternative done

## Switch to release/debug compilation ##
# The macro to do this is called ``_DEBUG'' to fit how Windows does. Sorry.
ifeq ($(findstring release, $(COMPILKIND)), release)

	PREPROCESSORFLAGS += -U_DEBUG 
	CFLAGS += -O2
	CXXFLAGS += -O2

else

	PREPROCESSORFLAGS += -D_DEBUG=1 
	PREPROCESSORFLAGS += -DXP_UNIX 
	PREPROCESSORFLAGS += -DVIDEO_SUNPLUS
#	PREPROCESSORFLAGS += -DTDC_SI_INTDEVICEDLL

	

ifneq ($(findstring kernel, $(COMPILKIND)), kernel) # -g only in user compilations
ifeq "$(CCOMPILER)" "gcc"
ifeq "$(CROSS)" ""
#	CFLAGS += -gdwarf-2 -g3
#	CXXFLAGS += -gdwarf-2 -g3
	CFLAGS += -g
	CXXFLAGS += -g
endif
else
	CFLAGS += -g
	CXXFLAGS += -g
endif
endif 
endif # -g only in user compilations

### Platform Option
ifdef RMPLATFORM
	PREPROCESSORFLAGS += -DRMPLATFORM=$(RMPLATFORM)
endif

CFLAGS += $(PREPROCESSORFLAGS) $(CWARNINGS)
## kernel code is always in C
CXXFLAGS += $(PREPROCESSORFLAGS) $(CXXWARNINGS) 

#########################################################
## select compilation according to COMPILKIND variable ##
#########################################################

TARGET_TYPE := $(strip $(TARGET_TYPE))

ifeq "$(TARGET_TYPE)" "LIBRARY"
ifeq ($(findstring static, $(COMPILKIND)), static)
TARGET_TYPE :=  OBJECT_LIBRARY
else
TARGET_TYPE :=  SHARED_LIBRARY
endif
endif

ifeq ($(findstring static, $(COMPILKIND)), static)
STATIC_LINKS_WITH += $(LINKS_WITH)
LDFLAGS += -static
else
ifeq ($(findstring implicit, $(COMPILKIND)), implicit)
IMPLICIT_LINKS_WITH := $(LINKS_WITH)
else
DEPENDS_ON += $(LINKS_WITH)
endif
endif

####################################################
## Describes the rule to make each type of target ##
####################################################
ifdef OBJECT_LIBRARY_USE_AR
OBJECT_LIBRARY = $(AR) rc $@ $^
else
OBJECT_LIBRARY = $(LD) -r $^ -o $@
endif
SHARED_LIBRARY = $(LINKTOOL) -shared $^ $(LDFLAGS) -o $@
EXECUTABLE = $(LINKTOOL) $^ $(LDFLAGS) $(EXEFLAGS) -o $@ && chmod a+x $@ && $(STRIP) $@
DEPEND_COMMAND = $(CROSS)gcc $(MUM_KI) $(PREPROCESSORFLAGS) -MM -E $(SRC) $(MAIN_SRC)
BINARY_FILE = $(OBJCOPY) -O binary $< $@

define MICROCODE
	$(SPASM) $(SPASMFLAGS) $^ -o $(UCODE_NAME).bin
        $(SPBIN) $(SPBINFLAGS) -h $(UCODE_NAME).bin
	$(call BIN_TO_H, $(UCODE_NAME), $@)
endef

###################################
## Describes ugly shell commands ##
###################################

COPY_FILE_LIST = sed 's/^.*://' | sed 's/\\$$//' | tr ' ' '\n' | sed '/^$$/d' | \
		sed '/^\//d' | \
		sed 's/^\([^/]\)/$(subst /,\/,$(LOCALDIR))\/\1/' | \
		sed '{ : rm_dotdot s/[^/]*\/\.\.\/// ; t rm_dotdot }' | sort -u | \
		xargs cp -f --parents --target-directory=$(1)

COPY_FILE_LIST2 = sed 's/^.*://' | sed 's/\\$$//' | tr ' ' '\n' | sed '/^$$/d' | \
        sed '/^\//d' | sort -u | \
        xargs -l1 cp -f --parents --target-directory=$(1)

REDUCE_DIR_TREE = cd $(1) ; x=1 ; while [ $$x -eq 1 ] ; \
		do x=0 ; [ `ls -1 | wc -l` -eq 1 -a -d `ls -1 | head -n 1` ] && \
		y=`ls -1` && mv $$y/* . && rmdir $$y && x=1 ; \
		done ; exit 0

BIN_TO_H = (echo "RMuint8 $(1)_bin[" ; \
		wc -c < $(1)_release.bin ; \
		echo "] = {"; \
	    hexdump -v -e '"\t" 8/1 "0x%02x, " "\n"' $(1)_release.bin; \
	    echo "};") \
	   | perl -pe 's/0x\s*,//mg' >$(2)

#########################################################
## Compute the local object files and the dependencies ## 
#########################################################

SRC := $(strip $(SRC))
MAIN_SRC := $(strip $(MAIN_SRC))
OBJ := $(addsuffix .o, $(basename $(SRC)))
ifeq "$(TARGET_TYPE)" "MICROCODE"
ifdef CLEAN_UCODE
# NOTE: need the space there ".o " to avoid replacing aac.ovlprog.asm to aac_$VERSION.ovlprog_$VERSION.o
OBJ := $(OBJ) # end of line
OBJ := $(subst .o ,_$(subst .,_,$(SPASM_VERSION)).o , $(OBJ))
endif
endif
MAIN_OBJ := $(addsuffix .o, $(basename $(MAIN_SRC)))
EXE := $(basename $(MAIN_SRC))
SUBDIR := $(dir $(DEPENDS_ON) $(STATIC_LINKS_WITH) $(IMPLICIT_LINKS_WITH))

ifneq ($(findstring clean, $(MAKECMDGOALS)) ,clean)	# if target is not clean
	ifneq "$(SRC) $(MAIN_SRC)" " "			# if SRC or MAIN_SRC are not void
		DEPEND := $(shell cat $(DEPEND_FILE) 2>/dev/null)   # take the dependencies in DEPEND_FILE 
		DEPEND := $(subst \, ,$(DEPEND))        # remove the back-slash
		DEPEND := $(subst :, , $(DEPEND))       # remove the :
		DEPEND := $(filter-out %.o, $(DEPEND))  # remove the .o file names
		DEPEND := $(SRC) $(MAIN_SRC) $(DEPEND)  # add SRC and MAIN_SRC in case not present before
		DEPEND := $(sort $(DEPEND))             # remove doublons 
	endif
endif

ifeq "$(TARGET_TYPE)" "OBJFILE"
TARGET := $(OBJ)
endif

ifeq "$(TARGET_TYPE)" "EXECUTABLE"
TARGET := $(EXE)
endif

ifeq "$(TARGET_TYPE)" "MICROCODE"
ifneq ($(findstring _bin.h, $(TARGET)), _bin.h)
$(error target name is not valid (must end with _bin.h))
endif
UCODE_NAME := $(subst _bin.h,, $(TARGET))
ifdef CLEAN_UCODE
TMP_FILES += $(addprefix $(UCODE_NAME), .log .lst .map .prof .bin _release.bin _labels.h _structures.h)
endif
ifeq ($(findstring clean, $(MAKECMDGOALS)) ,clean)
ifndef CLEAN_UCODE
TARGET := 
endif
endif
endif

#################################################################
## put here the targets you want to be updated anytime you ask ##
#################################################################

.PHONY: cleanall clean dist localdist reduce_dist api $(SUBDIR) all local prebuild postbuild copy_shared

#############################
## rule to make the target ##
#############################

all: prebuild $(SUBDIR) $(TARGET) postbuild

local: prebuild $(TARGET) postbuild

ifeq "$(TARGET_TYPE)" "SHARED_LIBRARY"
copy_shared: $(TARGET) $(SUBDIR)
	[ -d $(LIBDIR) ] && cp -f $(TARGET) $(LIBDIR)
else
copy_shared: $(SUBDIR)
endif

ifeq "$(TARGET_TYPE)" "OBJFILE"
copy_static: $(TARGET) $(SUBDIR)
	[ -d $(LIBDIR) ] && cp -f $(TARGET) $(LIBDIR)
endif

ifeq "$(TARGET_TYPE)" "OBJECT_LIBRARY"
$(TARGET): $(OBJ) $(STATIC_LINKS_WITH) $(EXTERNAL_STATIC_LINKS_WITH)
	$(OBJECT_LIBRARY)

copy_static: $(TARGET) $(SUBDIR)
	[ -d $(LIBDIR) ] && cp -f $(TARGET) $(LIBDIR)
endif

ifeq "$(TARGET_TYPE)" "SHARED_LIBRARY"
$(TARGET): $(OBJ) $(STATIC_LINKS_WITH) $(EXTERNAL_STATIC_LINKS_WITH) $(IMPLICIT_LINKS_WITH)
	$(SHARED_LIBRARY)
endif

ifeq "$(TARGET_TYPE)" "EXECUTABLE"
$(TARGET): %: %.o $(OBJ) $(STATIC_LINKS_WITH) $(EXTERNAL_STATIC_LINKS_WITH) $(IMPLICIT_LINKS_WITH)
	$(EXECUTABLE)
endif

ifeq "$(TARGET_TYPE)" "MICROCODE"
ifneq ($(findstring $(SRC), $(wildcard $(SRC))), $(SRC))
ifneq ($(findstring $(TARGET), $(wildcard $(TARGET))), $(TARGET))
$(error $(TARGET) is missing along with the source files!!!)
else
$(TARGET):
	$(MICROCODE)
endif
else
$(TARGET): $(SRC)
	$(MICROCODE)
endif
endif

ifeq "$(TARGET_TYPE)" "BINARY_FILE"
$(TARGET): $(EXE)
	$(BINARY_FILE)

$(EXE): %: %.o $(OBJ) $(STATIC_LINKS_WITH) $(EXTERNAL_STATIC_LINKS_WITH) $(IMPLICIT_LINKS_WITH)
	$(EXECUTABLE)
endif

$(STATIC_LINKS_WITH): $(@D)

$(IMPLICIT_LINKS_WITH): $(@D)

$(DEPENDS_ON): $(@D)

$(SUBDIR):
	@ echo ---- Subdir $(SRCDIR)$@
	$(MAKE) $(MAKEARGS) -C $@ $(MAKECMDGOALS) SRCDIR=$(SRCDIR)$@

$(SRC):

$(MAIN_SRC):

########################################################
## rules to execute a prebuild or a postbuild command ##
########################################################

prebuild:
ifneq "$(PREBUILD)" ""
	@echo "------START PREBUILD STEP-------"
	$(PREBUILD)
	@echo "------FINISH PREBUILD STEP------"
endif

postbuild:
ifneq "$(POSTBUILD)" ""
	@echo "------START POSTBUILD STEP-------"
	$(POSTBUILD)
	@echo "------FINISH POSTBUILD STEP------"
endif

######################################################
## include and compute if necessary the DEPEND_FILE ##
######################################################


$(DEPEND_FILE): $(DEPEND) $(LOCAL_MAKEFILE)
ifneq "$(TARGET_TYPE)" "MICROCODE"
ifneq "$(SRC) $(MAIN_SRC)" " " 
	$(DEPEND_COMMAND) | sed 's/:/: $(LOCAL_MAKEFILE)/' > $@
endif
endif

ifneq ($(findstring clean, $(MAKECMDGOALS)) ,clean)
-include $(DEPEND_FILE)
endif

######################################################
## copy all the needed files to compile the project ##
######################################################

dist: localdist $(SUBDIR)

dist2: localdist2 $(SUBDIR)

localdist:
	@[ -d $(DISTDIR) ] && \
	echo $(EXTRA_DIST_FILES) | cat - $(DEPEND_FILE) 2>/dev/null | $(call COPY_FILE_LIST,$(DISTDIR)) 

localdist2:
	@mkdir -p $(DISTDIR)/$(SRCDIR) && \
	echo $(EXTRA_DIST_FILES) | cat - $(DEPEND_FILE) 2>/dev/null | $(call COPY_FILE_LIST2,$(DISTDIR)/$(SRCDIR))

reduce_dist:
	@$(call REDUCE_DIR_TREE,$(DISTDIR))

api:
	@[ -d $(APIDIR) ] && \
	$(DEPEND_COMMAND) | $(call COPY_FILE_LIST,$(APIDIR)) 
	@$(call REDUCE_DIR_TREE,$(APIDIR))
	mipsel-linux-strip test/mginit

##############################
## how to clean the project ##
##############################

cleanall: clean $(SUBDIR)

clean: 
	$(RM) $(OBJ) $(MAIN_OBJ) $(TARGET) $(DEPEND_FILE) $(TMP_FILES) *~ 
