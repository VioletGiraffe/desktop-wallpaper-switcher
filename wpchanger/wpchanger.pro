TARGET = wpchanger
DESTDIR = ../bin
TEMPLATE = lib
CONFIG += staticlib

QT = core gui

OBJECTS_DIR = ../build/wpchanger
MOC_DIR     = ../build/wpchanger
UI_DIR      = ../build/wpchanger
RCC_DIR     = ../build/wpchanger

HEADERS += \
	src/wallpaperchanger.h \
	src/settings.h \
	src/imagelist.h

SOURCES += \
	src/wallpaperchanger.cpp \
	src/imagelist.cpp

INCLUDEPATH += \
	../image/src \
	../utility   \
	../qtutils \
	../cpputils

LIBS += -L../bin -limage

DEFINES += NO_QTUTILS_WIDGETS

win*{
	QMAKE_CXXFLAGS += /MP
	CXXFLAGS += /favor:blend
	DEFINES += WIN32_LEAN_AND_MEAN NOMINMAX
}
