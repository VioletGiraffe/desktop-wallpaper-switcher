TARGET = image
DESTDIR = ../bin
TEMPLATE = lib
CONFIG += staticlib

QT = gui core

OBJECTS_DIR = ../build/image
MOC_DIR     = ../build/image
UI_DIR      = ../build/image
RCC_DIR     = ../build/image

HEADERS += \
	src/image.h

SOURCES += \
	src/image.cpp

win*{
	QMAKE_CXXFLAGS += /MP
	CXXFLAGS += /favor:blend
	DEFINES += WIN32_LEAN_AND_MEAN NOMINMAX
}
