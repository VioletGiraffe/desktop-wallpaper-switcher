TARGET   = WallpaperSwitcher
DESTDIR  = ../bin
TEMPLATE = app

OBJECTS_DIR = ../build/app
MOC_DIR     = ../build/app
UI_DIR      = ../build/app
RCC_DIR     = ../build/app

mac* | linux*{
    CONFIG(release, debug|release):CONFIG += Release
    CONFIG(debug, debug|release):CONFIG += Debug
}

Release:OUTPUT_DIR=release
Debug:OUTPUT_DIR=debug

QT = gui core

#check Qt version
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -L../bin -L../bin/$${OUTPUT_DIR} -lutility -limage -lwpchanger -lqtutils -lcpputils

INCLUDEPATH +=  ../image/src \
				../wpchanger/src \
				../utility \
				../qtutils \
				../cpputils \
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
