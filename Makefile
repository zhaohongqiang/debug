TARGET_TYPE := EXECUTABLE
MAIN_SRC := main.c
#MAIN_SRC := VodDesk.c
SRC := \
	js/tdmalloc.c\
	comserv/tdstr.c\
	comserv/tdstring.c \
	comserv/tdarray.c \
	tdevice/file.c	
export COMPILKIND += withthreads withdl
USE_STD_LIB := 1
RMCFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -g -W -I./include/
#RMCFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -g -W -funwind-tables -rdynamic -I./include/
LDFLAGS +=  -L./lib -lpthread -ldl -lm -lrt 
include inc.Makefile

install:
	cp test/main /share/mginit -f
