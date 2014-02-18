TARGET = utility
TEMPLATE = lib
DESTDIR = ../bin
CONFIG += staticlib

OBJECTS_DIR = ../build/utility
MOC_DIR = ../build/utility
UI_DIR = ../build/utility
RCC_DIR = ../build/utility

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
