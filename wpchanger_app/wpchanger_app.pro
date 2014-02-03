DESTDIR  = ../bin
TEMPLATE = app
TARGET   = WallpaperChanger

OBJECTS_DIR = ../build
MOC_DIR     = ../build
UI_DIR      = ../build
RCC_DIR     = ../build

QT = gui core

#check Qt version
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -L../bin  -lutility -limage -lwpchanger -lqtutils

INCLUDEPATH +=  ../image/src \
				../wpchanger/src \
				../utility \
				../qtutils \
				src/qt/

win*{
	QMAKE_CXXFLAGS += /openmp /MP
	CXXFLAGS += /favor:blend
	DEFINES += WIN32_LEAN_AND_MEAN NOMINMAX
}

include (src/qt/app.pri)
include (src/qt/aboutdialog/aboutdialog.pri)
include (src/qt/thumbnailwidget/thumbnail.pri)
include (src/qt/imagelist/imagelist.pri)
include (src/qt/thumbnailexplorer/thumbnailexplorer.pri)
