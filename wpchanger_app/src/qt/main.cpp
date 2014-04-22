#include <QApplication>

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
