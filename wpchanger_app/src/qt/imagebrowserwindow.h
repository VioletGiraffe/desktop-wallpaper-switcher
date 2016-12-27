#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QMainWindow>
RESTORE_COMPILER_WARNINGS

class ImageList;
class QListWidgetItem;

namespace Ui {
class ImageBrowserWindow;
}

class WallpaperChanger;

class ImageBrowserWindow : public QMainWindow
{

public:
	explicit ImageBrowserWindow(QWidget *parent = 0);
	~ImageBrowserWindow();

	bool populate();

	//Delete selected images from disk (and from the list if successful)
	void deleteSelectedImagesFromDisk();

protected:
	void showEvent(QShowEvent *event) override;
	void closeEvent(QCloseEvent *event) override;

private:
	void showContextMenu(const QPoint& pos);
	// Image double-clicked
	void itemActivated(QListWidgetItem * item);
	//
	void zoomIn();
	void zoomOut();

private:
	// Will also delete item!
	void removeItemFromView (QListWidgetItem * item);

	// Finds image index in the image list by its file path
	qulonglong imageIdByPath (const QString& path) const;

private:
	WallpaperChanger       & _wpChanger;
	Ui::ImageBrowserWindow * ui;
};
