TARGET = utility
TEMPLATE = lib
DESTDIR = ../bin
CONFIG += staticlib

OBJECTS_DIR = ../build
MOC_DIR = ../build
UI_DIR = ../build
RCC_DIR = ../build

#DEFINES += UTILITY_LIB

HEADERS += \
	utility_lib.h

include (signals/signals.pri)
include (time/time.pri)

win*{
	QMAKE_CXXFLAGS += /MP
	CXXFLAGS += /favor:blend
	DEFINES += WIN32_LEAN_AND_MEAN NOMINMAX
}
