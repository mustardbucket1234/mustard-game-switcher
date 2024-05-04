# A configuration file for testing

VERSION = 0.3

# paths
OUTPUT_DIR = output/x86
ASSETS_DIR = assets
SRC_DIR = src
OBJ_DIR = $(OUTPUT_DIR)/obj


PREFIX = /opt/rg35xx

# compiler and linker
CC = g++

# includes and libs
INCS = -I. -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT -DRG35XX
LIBS = -lc -L/usr/lib -lSDL2 -lSDL2_ttf -lSDL2_image -lpthread -Wl,-Bstatic,-lutil,-Bdynamic

# flags
CPPFLAGS = -DVERSION=\"${VERSION}\" -fPIC
CPPFLAGS += -DDEBUG
CFLAGS += -Os -Wall ${INCS} ${CPPFLAGS} -std=gnu11
LDFLAGS += ${CFLAGS} ${LIBS} -lSDL -s