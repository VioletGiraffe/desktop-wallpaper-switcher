#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "aboutdialog/aboutdialog.h"
#include "imagelist/qtimagelistitem.h"
#include "thumbnailwidget/imagethumbnailwidget.h"
#include "signals/signal.h"
#include "imagebrowserwindow.h"

#include <QMainWindow>
#include <QLabel>
#include <QSystemTrayIcon>
#include <vector>
#include <map>

namespace Ui {
	class MainWindow;
}

class QLabel;
class WallpaperChanger;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	//Deletes all the items of the tree, freeing the memory
	void clearImageList();

	void loadGeometry();

protected:
	virtual bool event(QEvent * event);
	virtual void closeEvent(QCloseEvent * e);

private slots:
	// Select duplicate entries in the list
	void selectDuplicateEntries();
	// Find and select duplicate files on disk
	void findDuplicateFiles();
	// Remove non-existent images from list
	void removeNonExistingEntries();
	// Removes current wallpaper from list
	void removeCurrentWp();
	// Deletes current wallpaper from disk
	void deleteCurrentWp();
	// Request to show "About" window
	void onActionAboutTriggered();
	// Triggers a dialog window to add images to a list
	void onAddImagesTriggered();
	//
	void onImgSelected(QTreeWidgetItem* current, QTreeWidgetItem* prev);

	void onWPDblClick(QModelIndex);

	// Wallpaper display mode changed
	void displayModeChanged(int mode);

	// Image browser requested
	void openImageBrowser();

	void showImageListContextMenu (const QPoint& pos);

	// Opens settings dialog
	void openSettings();

// Save-load
	// Ctrl+Shift+S
	void saveImageListAs();
	// Ctrl+S
	void saveImageList();
	// Ctrl+L
	void loadImageList();

// Switching
	void nextWallpaper();
	void previousWallpaper();
	void stopSwitching();
	void resumeSwitching();

// Interface
	void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
	void restoreWindow();
	void trayIconMenu();

// Key pressed
	void keyPressEvent(QKeyEvent * e);

private:
	void selectImage (qulonglong id);

	void dropEvent(QDropEvent*);
	void dragEnterEvent(QDragEnterEvent *event);

	//Displays a given message in the status bar
	void setStatusBarMessage(const QString& msg);

	//Remove selected images from the list
	void removeSelectedImages();
	//Delete selected images from disk (and from the list if successful)
	void deleteSelectedImagesFromDisk();

	//Displays info for image at specified index
	void displayImageInfo(size_t imageIndex);

	//Updates the contents of image list control according to _wpChanger::_imageList
	void updateImageList(bool totalUpdate);

	void addImagesFromDirecoryRecursively(const QString& dir);

	void promptToSaveList();

// Slots
	// Image list was updated
	void imageListChanged(size_t index);
	// Image list was cleared
	void imageListCleared();
	// Current wallpaper changed
	void wallpaperChanged(size_t index);
	// Time until next switch
	void timeToNextSwitch(size_t seconds);

// UI setup
	void initToolbar();


private:
	Ui::MainWindow              * ui;
	AboutDialog                 * _aboutDlg;
	QSystemTrayIcon               _trayIcon;

	ImageBrowserWindow            _browserWindow;

	std::vector<QtImageListItem>  _itemsList;
	WallpaperChanger            & _wpChanger;

	QString                       _currentListFileName;
	QLabel                        _statusBarMsgLabel;
	QLabel                        _statusBarTimeToSwitchLabel;
	QLabel                        _statusBarNumImages;

	size_t                        _timeToSwitch;

	Slot                          _slotImageListChanged;
	Slot                          _slotImageListCleared;
	Slot                          _slotWallpaperChanged;
	Slot                          _slotTimeToNextSwitchChanged;

	bool                          _bListSaved;
	size_t                        _previousListSize; // Is used to determine that the list was edited

	std::map<qulonglong /*id*/, QtImageListItem* /*item*/> _imageListWidgetItems;
};

#endif // MAINWINDOW_H
