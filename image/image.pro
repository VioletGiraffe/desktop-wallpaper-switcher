DESTDIR = ../bin
TEMPLATE = lib
CONFIG += staticlib

QT = gui core

OBJECTS_DIR = ../build
MOC_DIR     = ../build
UI_DIR      = ../build
RCC_DIR     = ../build

HEADERS += \
	src/image.h \
    src/sha1.h

SOURCES += \
	src/image.cpp \
    src/sha1.cpp

win*{
	QMAKE_CXXFLAGS += /MP
	CXXFLAGS += /favor:blend
	DEFINES += WIN32_LEAN_AND_MEAN NOMINMAX
}
