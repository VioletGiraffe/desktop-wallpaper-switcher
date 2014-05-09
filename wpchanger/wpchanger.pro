DESTDIR = ../bin
TEMPLATE = lib
CONFIG += staticlib

QT = core gui

OBJECTS_DIR = ../build
MOC_DIR = ../build
UI_DIR = ../build
RCC_DIR = ../build

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
	../qtutils

LIBS += -L../bin -limage

DEFINES += NO_QTUTILS_WIDGETS

win*{
	QMAKE_CXXFLAGS += /MP
	CXXFLAGS += /favor:blend
	DEFINES += WIN32_LEAN_AND_MEAN NOMINMAX
}
