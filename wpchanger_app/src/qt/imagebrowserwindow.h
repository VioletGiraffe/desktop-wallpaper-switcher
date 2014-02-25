#ifndef IMAGEBROWSERWINDOW_H
#define IMAGEBROWSERWINDOW_H

#include <QMainWindow>
#include <QShortcut>

class ImageList;
class QListWidgetItem;

namespace Ui {
class ImageBrowserWindow;
}

class WallpaperChanger;

class ImageBrowserWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit ImageBrowserWindow(QWidget *parent = 0);
	~ImageBrowserWindow();

	bool populate();

	//Delete selected images from disk (and from the list if successful)
	void deleteSelectedImagesFromDisk();

protected:
	virtual bool event(QEvent * event);

private slots:
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
	qulonglong imageIdByPath (QString path) const;

private:
	QShortcut              * _zoomInShortcut, * _zoomOutShortcut;
	WallpaperChanger       & _wpChanger;
	Ui::ImageBrowserWindow * ui;
};

#endif // IMAGEBROWSERWINDOW_H
