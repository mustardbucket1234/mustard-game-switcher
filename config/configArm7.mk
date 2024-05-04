# Cross Compiler Config for Linux x86 -> RG35XX
# Install the RG35xx+ Batocera toolchain from here:
# https://github.com/rg35xx-cfw/rg35xx-cfw.github.io/releases/tag/rg35xx_plus_h_sdk_20240207
# Extract the toolchain to /opt/rg35xx

VERSION = 0.3

# paths
OUTPUT_DIR = output/arm7
ASSETS_DIR = assets
SRC_DIR = src
OBJ_DIR = $(OUTPUT_DIR)/obj


PREFIX = /opt/rg35xx
MANPREFIX = ${PREFIX}/share/man
CROSS_COMPILE ?= ${PREFIX}/bin/arm-linux-

# compiler and linker
CC = ${CROSS_COMPILE}g++
SYSROOT = $(shell ${CC} --print-sysroot)

# includes and libs
INCS = -I. -I${SYSROOT}/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT -DRG35XX
LIBS = -lc -L${SYSROOT}/usr/lib -lSDL2 -lSDL2_ttf -lSDL2_image -lpthread -Wl,-Bstatic,-lutil,-Bdynamic

# flags
CPPFLAGS = -DVERSION=\"${VERSION}\" -fPIC
CFLAGS += -Os -Wall ${INCS} ${CPPFLAGS} -fPIC -std=gnu11 -marm -mtune=cortex-a9 -mfpu=neon-fp16 -mfloat-abi=hard -march=armv7-a+neon-fp16 -flto
LDFLAGS += ${CFLAGS} ${LIBS} -lSDL -s
