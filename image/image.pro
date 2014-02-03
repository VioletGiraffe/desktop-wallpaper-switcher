DESTDIR = ../bin
TEMPLATE = lib
CONFIG += staticlib

QT = gui core

OBJECTS_DIR = ../build
MOC_DIR     = ../build
UI_DIR      = ../build
RCC_DIR     = ../build

HEADERS += \
	src/image.h

SOURCES += \
	src/image.cpp

win*{
	QMAKE_CXXFLAGS += /MP
	CXXFLAGS += /favor:blend
	DEFINES += WIN32_LEAN_AND_MEAN NOMINMAX
}
