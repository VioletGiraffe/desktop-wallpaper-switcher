#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "wallpaperchanger.h"
#include "imagelist/qtimagelistitem.h"
#include "thumbnailwidget/imagethumbnailwidget.h"
#include "imagebrowserwindow.h"

#include "imagelist/cfilterdialog.h"

#include <QLabel>
#include <QMainWindow>
#include <QProgressBar>
#include <QSystemTrayIcon>

#include <vector>
#include <map>

namespace Ui {
	class MainWindow;
}

class WallpaperChanger;

class MainWindow : public QMainWindow, public WallpaperWatcher
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
	// Launch the search UI
	void search();
	// Search the list by given filename pattern
	void searchByFilename(QString name);
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

// Key pressed
	void keyPressEvent(QKeyEvent * e);

// Progress notifications
	void updateProgress(int percent, bool show, QString text);


// Slots
	// Image list was updated
	void listChanged(size_t index) override;
	// Image list was cleared
	void listCleared() override;
	// Current wallpaper changed
	void wallpaperChanged(size_t index) override;
	// Time until next switch
	void timeToNextSwitch(size_t seconds) override;
	void wallpaperAdded(size_t) override;

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

	void updateWindowTitle();

// UI setup
	void initToolbar();

signals:
	// To update UI from within worker threads
	void signalUpdateProgress(int percent, bool show, QString text);


private:
	Ui::MainWindow              * ui;
	QSystemTrayIcon               _trayIcon;
	CFilterDialog                 _imageListFilterDialog;

	ImageBrowserWindow            _browserWindow;

	std::vector<QtImageListItem>  _itemsList;
	WallpaperChanger            & _wpChanger;

	QString                       _currentListFileName;
	QLabel                        _statusBarMsgLabel;
	QLabel                        _statusBarTimeToSwitchLabel;
	QProgressBar                  _progressBar;
	QLabel                        _statusBarNumImages;

	size_t                        _timeToSwitch;

	bool                          _bListSaved;
	size_t                        _previousListSize; // Is used to determine that the list was edited

	std::map<qulonglong /*id*/, QtImageListItem* /*item*/> _imageListWidgetItems;
};

#endif // MAINWINDOW_H
