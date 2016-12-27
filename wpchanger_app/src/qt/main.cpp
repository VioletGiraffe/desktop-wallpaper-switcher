#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QApplication>
RESTORE_COMPILER_WARNINGS

#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setOrganizationName("VGSoft");
	a.setApplicationName("WPChanger");

	MainWindow w;
	w.loadGeometry();
	w.show();

	return a.exec();
}
