#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "imagelist/qtimagelistitem.h"
#include "settingsdialog.h"
#include "settings.h"
#include "wallpaperchanger.h"
#include "settings/csettings.h"

#include <QFileDialog>
#include <QTreeView>
#include <QUrl>
#include <QFile>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMessageBox>
#include <QtAlgorithms>
#include <QDebug>
#include <QMimeData>
#include <QString>
#include <QHeaderView>
#include <QDesktopServices>
#if QT_VERSION >= QT_VERSION_CHECK (5,0,0)
#include <QStandardPaths>
#endif

#include <assert.h>
#include "time/ctime.h"

#ifdef WIN32
#include <windows.h>
#pragma comment(lib, "user32.lib")
#endif

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	_aboutDlg(new AboutDialog(this)),
	_currentlySelectedImageIndex (0),
	_timeToSwitch(0),
	_currentListFileName(CSettings().value(SETTINGS_IMAGE_LIST_FILE).toString()),
	_trayIcon(QApplication::style()->standardIcon(QStyle::SP_MediaStop), this),
	_wpChanger (WallpaperChanger::instance()),
	_bListSaved (true),
	_previousListSize(0)
{
	ui->setupUi(this);

	initToolbar();

	ui->_filePathIndicator->setVisible(false);

	_trayIcon.setParent(this);
	_trayIcon.show();
	connect(&_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));


	ui->_imageList->sortByColumn(FileNameColumn, Qt::AscendingOrder);

	ui->statusBar->addWidget(&_statusBarMsgLabel);

	_statusBarTimeToSwitchLabel.setAlignment(Qt::AlignRight);
	_statusBarTimeToSwitchLabel.setToolTip("Time until next switch");
	ui->statusBar->addWidget(&_statusBarTimeToSwitchLabel);

	ui->statusBar->addWidget(&_statusBarNumImages);

	connect(ui->actionAdd_Images,SIGNAL(triggered()), SLOT(onAddImagesTriggered()));
	connect(ui->_imageList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), SLOT(onImgSelected(QTreeWidgetItem*,QTreeWidgetItem*)));
	connect(ui->_imageList, SIGNAL(doubleClicked(QModelIndex)), SLOT(onWPDblClick(QModelIndex)));
	connect(ui->actionSave_Image_List, SIGNAL(triggered()), SLOT(saveImageList()));
	connect(ui->actionSave_Image_List_As, SIGNAL(triggered()), SLOT(saveImageListAs()));
	connect(ui->actionLoad_Image_List, SIGNAL(triggered()), SLOT(loadImageList()));
	connect(ui->_wpModeComboBox, SIGNAL(activated(int)), SLOT(displayModeChanged(int)));
	connect(ui->actionBrowser, SIGNAL(triggered()), SLOT(openImageBrowser()));
	connect(ui->actionSettings, SIGNAL(triggered()), SLOT(openSettings()));
	connect(ui->actionFind_duplicate_files_on_disk, SIGNAL(triggered()), SLOT(findDuplicateFiles()));
	connect(ui->actionFind_duplicate_list_entries, SIGNAL(triggered()), SLOT(selectDuplicateEntries()));
	connect(ui->actionRemove_Non_Existent_Entries, SIGNAL(triggered()), SLOT(removeNonExistingEntries()));
	connect(ui->actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));

	setAcceptDrops(true);

	_slotImageListChanged = _wpChanger._signalListChanged.connect(this, &MainWindow::imageListChanged);
	_slotWallpaperChanged = _wpChanger._signalWallpaperChanged.connect(this, &MainWindow::wallpaperChanged);
	_slotTimeToNextSwitchChanged = _wpChanger._signalTimeToNextSwitch.connect(this, &MainWindow::timeToNextSwitch);
	timeToNextSwitch(_wpChanger.interval());

	connect(ui->_imageList, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(showImageListContextMenu(const QPoint&)));

	const QString listFileName (CSettings().value(SETTINGS_IMAGE_LIST_FILE, "").toString());
	if (!listFileName.isEmpty())
		_wpChanger.loadList(listFileName);
	else
		_statusBarMsgLabel.setText("Start by dragging and dropping images");

	const uint wpIndex = CSettings().value(SETTINGS_CURRENT_WALLPAPER, std::numeric_limits<uint>().max()).toUInt();
	if (wpIndex < std::numeric_limits<uint>().max())
		wallpaperChanged((size_t)wpIndex);

	_previousListSize = _wpChanger.numImages();

	ui->_wpModeComboBox->setEnabled(false);
}

MainWindow::~MainWindow()
{
	delete _aboutDlg;
	delete ui;
}

//Displays a given message in the status bar
void MainWindow::setStatusBarMessage( const QString& msg )
{
	_statusBarMsgLabel.setText(msg);
}

//Updates the contents of image list control according to _wpChanger::_imageList
void MainWindow::updateImageList()
{
	disconnect(ui->_imageList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(onImgSelected(QTreeWidgetItem*,QTreeWidgetItem*)));
	CTime start;

	int skipped = 0;

	if (_wpChanger.numImages() > 0 && _previousListSize != 0)
	{
		if (_bListSaved && _previousListSize != _wpChanger.numImages())
			_bListSaved = false;
		_previousListSize = _wpChanger.numImages();
	}

	decltype(_imageListWidgetItems) newImageListWidgetItems;
	for (size_t i = 0; i < _wpChanger.numImages(); ++i)
	{
		const auto imageListItem = _imageListWidgetItems.find(_wpChanger.image(i).id());
		if (imageListItem == _imageListWidgetItems.end()) // This image is not yet in the widget, adding
		{
			QtImageListItem * item =  new QtImageListItem(_wpChanger.image(i), i, _wpChanger.currentWallpaper() == i);
			if (!_wpChanger.imageExists(i))
			{
				for (int i = 0; i < ui->_imageList->columnCount(); ++i)
				{
					item->setBackgroundColor(i, QColor(Qt::red));
					item->setTextColor(i, QColor(Qt::white));
				}
			}

			newImageListWidgetItems[_wpChanger.image(i).id()] = item;
			ui->_imageList->addTopLevelItem(item);
		}
		else // An entry for this image has already been added, no need to re-create it
		{
			imageListItem->second->setCurrent(false);
			newImageListWidgetItems[imageListItem->first] = imageListItem->second;
			_imageListWidgetItems.erase(imageListItem);
			++skipped;
		}
	}

	// Cleaning up entries for no longer existing images
	for (auto it = _imageListWidgetItems.begin(); it != _imageListWidgetItems.end(); ++it)
	{
		delete it->second; // This both deletes the item and removes it from the widget: http://qt-project.org/doc/qt-4.8/qtreewidgetitem.html#dtor.QTreeWidgetItem
	}

	// Marking the current WP
	assert(_wpChanger.currentWallpaper() == size_t_max || newImageListWidgetItems.count(_wpChanger.image(_wpChanger.currentWallpaper()).id()) > 0);
	if (_wpChanger.currentWallpaper() != size_t_max)
		newImageListWidgetItems[_wpChanger.image(_wpChanger.currentWallpaper()).id()]->setCurrent();

	// Updating the list
	_imageListWidgetItems = newImageListWidgetItems;

	for (int column = 0; column < ui->_imageList->columnCount(); ++column)
	{
		ui->_imageList->resizeColumnToContents(column);
	}

	connect(ui->_imageList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(onImgSelected(QTreeWidgetItem*,QTreeWidgetItem*)));
	_statusBarNumImages.setText(QString ("%1 images in the list").arg(_wpChanger.numImages()));
	qDebug() << "Updating list of " << _wpChanger.numImages() << " items took " << (CTime() - start) / 1000.0f <<"sec" << ", skipped creating" << skipped << "entries";
}

void MainWindow::addImagesFromDirecoryRecursively(const QString& path)
{
	const QFileInfo info (path);
	if (info.exists() /*&& path != ".." && path != "."*/)
	{
		if (info.isDir())
		{
			const QDir dir (path);
			const QFileInfoList entries = dir.entryInfoList();
			for (int i = 0; i < entries.size(); ++i)
			{
				const QString newPath = entries[i].absoluteFilePath();
				if (newPath != info.absoluteFilePath() && newPath.size() > info.absoluteFilePath().size()) // ???
					addImagesFromDirecoryRecursively(entries[i].absoluteFilePath());
			}
		}
		else if (WallpaperChanger::isSupportedImageFile(path))
		{
			//Attempt to add and indicate the result
			if (!_wpChanger.addImage(path))
				setStatusBarMessage(path + " : " + "failed to open as image");
		}
	}
}

void MainWindow::promptToSaveList()
{
	if (QMessageBox::question(this, "Save changes?", "The image list was modified, do you want to save changes?", QMessageBox::Save | QMessageBox::No) == QMessageBox::Save)
		saveImageList();
}

//Deletes all the items of the tree, freeing the memory
void MainWindow::clearImageList()
{
	//For all top-level items
	for (int i = 0; i < ui->_imageList->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem* item = ui->_imageList->topLevelItem(i);
		//Deleting all child items
		for (int k = 0; k < item->childCount(); ++k)
		{
			if (item->child(k))
				delete item->child(k);
		}
		//Deleting the current top-level item
		delete item;
	}
	ui->_imageList->clear();
}

void MainWindow::loadGeometry()
{
	CSettings s;
	restoreGeometry( s.value(SETTINGS_WINDOW_GEOMETRY).toByteArray());
	restoreState( s.value(SETTINGS_WINDOW_STATE).toByteArray());
	ui->_imageList->header()->restoreGeometry(s.value(SETTINGS_LIST_GEOMETRY).toByteArray());
	ui->_imageList->header()->restoreState(s.value(SETTINGS_LIST_STATE).toByteArray());
}

bool MainWindow::event(QEvent * event)
{
	static bool firstShown = true;
	if(event->type() == QEvent::WindowStateChange && isMinimized())
		QTimer::singleShot(0, this, SLOT(hide()));
	else if (event->type() == QEvent::Show && firstShown)
	{
		firstShown = false;
		QTimer::singleShot(0, this, SLOT(hide()));
	}

	return QWidget::event(event);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
	if (e->type() == QCloseEvent::Close)
	{
		CSettings s;
		s.setValue(SETTINGS_WINDOW_GEOMETRY, saveGeometry());
		s.setValue(SETTINGS_WINDOW_STATE, saveState());
		s.setValue(SETTINGS_LIST_GEOMETRY, ui->_imageList->header()->saveGeometry());
		s.setValue(SETTINGS_LIST_STATE, ui->_imageList->header()->saveState());

		if (!_bListSaved)
			promptToSaveList();
	}
}

// Select duplicate entries in the list
void MainWindow::selectDuplicateEntries()
{
	std::vector<std::pair<size_t, QString> > vec;
	for (size_t i = 0; i < _wpChanger.numImages(); ++i)
	{
		vec.push_back(std::make_pair(i, _wpChanger.image(i).imageFilePath()));
	}

	std::sort(vec.begin(), vec.end(), [](std::pair<size_t, QString> a, std::pair<size_t, QString> b) { return a.second < b.second; });

	for (size_t i = 0; i < vec.size() - 1; ++i)
	{
		if (vec[i+1].second == vec[i].second)
			selectImage(vec[i].first);
	}
}

// Find and select duplicate files on disk
void MainWindow::findDuplicateFiles()
{
	std::vector<std::pair<size_t, int> > vec;
	for (size_t i = 0; i < _wpChanger.numImages(); ++i)
	{
		vec.push_back(std::make_pair(i, _wpChanger.image(i).params()._fileSize));
	}

	std::sort(vec.begin(), vec.end(), [](std::pair<size_t, int> a, std::pair<size_t, int> b) { return a.second < b.second; });

	for (size_t i = 0; i < vec.size() - 1; ++i)
	{
		if (vec[i+1].second == vec[i].second)
			selectImage(vec[i].first);
	}
}

void MainWindow::removeNonExistingEntries()
{
	_wpChanger.removeNonexistentEntries();
}

// Removes current wallpaper from list
void MainWindow::removeCurrentWp ()
{
	std::vector<size_t> v;
	v.push_back(_wpChanger.currentWallpaper());
	_wpChanger.removeImages(v);
}

// Deletes current wallpaper from disk
void MainWindow::deleteCurrentWp ()
{
	if (_wpChanger.currentWallpaper() != size_t_max)
	{
		std::vector<size_t> v;
		v.push_back(_wpChanger.currentWallpaper());
		_wpChanger.deleteImagesFromDisk(v);
	}
}

//Request to show "About" window
void MainWindow::onActionAboutTriggered()
{
	_aboutDlg->show();
}

//Triggers a dialog window to add images to a list
void MainWindow::onAddImagesTriggered()
{
#if QT_VERSION >= QT_VERSION_CHECK (5,0,0)
	const QString location = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
#else
	const QString location = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
#endif

	QStringList images = QFileDialog::getOpenFileNames(
		this,
		"Select one or more images to add",
		location,
		"Images (*.png *.bmp *.jpg *.jpeg *.gif)");

	if (! (images.empty() ))
	{
		CTime start;
		QStringList::const_iterator it = images.begin();
		for (; it != images.end(); ++it)
		{
			if (WallpaperChanger::isSupportedImageFile(*it))
				if (!_wpChanger.addImage(*it))
					setStatusBarMessage(*it + " : " + "failed to open as image");
		}
		ui->ImageThumbWidget->displayImage(images.back());
		ui->ImageThumbWidget->update();

		CTime stop;
		qDebug() << "Opening " << images.size() << "items took " << (stop - start) / 1000.0f <<"secs";
	}
}

void MainWindow::onImgSelected(QTreeWidgetItem* current, QTreeWidgetItem* /*prev*/)
{
	if (current)
		_currentlySelectedImageIndex = current->data(0, Qt::UserRole).toUInt();
	else
		return;

	if (_currentlySelectedImageIndex < _wpChanger.numImages())
	{
		ui->ImageThumbWidget->displayImage(_wpChanger.image(_currentlySelectedImageIndex));
		displayImageInfo(_currentlySelectedImageIndex);
	}
}

void MainWindow::dropEvent(QDropEvent * de)
{
	CTime start;
	const QMimeData * mimeData = de->mimeData();
	_wpChanger.enableListUpdateCallbacks(false);

	if (mimeData->urls().size() == 1)
	{
		const QString fileName = mimeData->urls().at(0).path().remove(0, 1);
		if (fileName.endsWith(".wil")) // It's a wallaper list
			_wpChanger.loadList(fileName);
	}
	else
	{
		//For every dropped file
		for (int urlIndex = 0; urlIndex < mimeData->urls().size(); ++urlIndex)
		{
			//Remove the heading '/
			const QString filename = mimeData->urls().at(urlIndex).path().remove(0, 1);
			addImagesFromDirecoryRecursively(filename);
		}
	}

	_wpChanger.enableListUpdateCallbacks(true);
	updateImageList();

	qDebug() << "Dropping " << mimeData->urls().size() << "Items took " << (CTime() - start) / 1000.0f <<"secs";
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasUrls())
	{
		event->acceptProposedAction();
	}
}

void MainWindow::onWPDblClick( QModelIndex )
{
	QTreeWidgetItem* item = ui->_imageList->currentItem();
	size_t idx = item->data(0, Qt::UserRole).toUInt();
	if ( !_wpChanger.setWallpaper(idx) )
	{
		setStatusBarMessage("Failed to set selected picture as a wallpaper");
	}
}

//Wallpaper display mode changed
void MainWindow::displayModeChanged (int mode)
{
	assert (mode >= 0 && mode <= SYSTEM_DEFAULT);

	QList<QTreeWidgetItem*> selected = ui->_imageList->selectedItems();
	const int numSelected = selected.size();
	for (int i = 0; i < numSelected; ++i)
	{
		const size_t itemIdx = (size_t)selected[i]->data(0, Qt::UserRole).toUInt();
		_wpChanger.image(itemIdx).setStretchMode(WPOPTIONS(mode));
	}
	_wpChanger.listChanged();
}

// Image browser requested
void MainWindow::openImageBrowser()
{
	_browserWindow.showMaximized();
}

void MainWindow::showImageListContextMenu(const QPoint& pos)
{
	if (ui->_imageList->selectedItems().isEmpty())
		return;

	const QPoint globalPos = ui->_imageList->viewport()->mapToGlobal(pos);

	QMenu menu;
	QAction * deleteFromDisk = menu.addAction("Delete from disk");
	QAction * view           = menu.addAction("View");
	QAction * openFolder     = menu.addAction("Open folder for browsing");

	QAction * selectedItem = menu.exec(globalPos);
	if (selectedItem == deleteFromDisk)
	{
		if (QMessageBox::question(this, "Are you sure?", "Are you sure you want to irreversibly delete the selected image(s) from disk?", QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
			deleteSelectedImagesFromDisk();
	}
	else if (selectedItem == view)
	{
		if (ui->_imageList->currentItem())
			QDesktopServices::openUrl(QUrl::fromLocalFile(_wpChanger.image((size_t)ui->_imageList->currentItem()->data(MarkerColumn, Qt::UserRole).toUInt()).imageFilePath()));
	}
	else if (selectedItem == openFolder)
	{
		if (ui->_imageList->currentItem())
			QDesktopServices::openUrl(QUrl::fromLocalFile(_wpChanger.image((size_t)ui->_imageList->currentItem()->data(MarkerColumn, Qt::UserRole).toUInt()).imageFileFolder()));
	}
	else if (selectedItem)
	{
		assert(!"Unhandled menu item activated");
	}
}

void MainWindow::openSettings()
{
	SettingsDialog().exec();
}

void MainWindow::displayImageInfo (size_t imageIndex)
{
	const Image& img = _wpChanger.image(imageIndex);
	ui->_filePathIndicator->setText(img.imageFilePath());
	ui->_imageDimensionsIndicatorLabel->setText(QString("%1x%2").arg(img.params()._width).arg(img.params()._height));
	ui->_imageSizeIndicatorLabel->setText(QString("%1 KB").arg(img.params()._fileSize / 1024));
	ui->_wpModeComboBox->setCurrentIndex(img.params()._wpDisplayMode);
	ui->_wpModeComboBox->setEnabled(true);
}

void MainWindow::saveImageListAs()
{
	_currentListFileName = QFileDialog::getSaveFileName(this, "Choose the file you want to save the image list to", QString(), "WallpaperChanger image list file (*.wil)");
	if (_currentListFileName.isEmpty())
		return;

	if (!_currentListFileName.contains(".wil"))
		_currentListFileName.append(".wil");

	if (!_wpChanger.saveList(_currentListFileName))
	{
		QMessageBox::information(this, "Error saving image list", "Error occurred while trying to save image list. Try saving to another file or folder.");
	}
	else
	{
		_bListSaved = true;
		CSettings().setValue(SETTINGS_IMAGE_LIST_FILE, _currentListFileName);
	}
}

void MainWindow::saveImageList()
{
	if (_currentListFileName.isEmpty())
	{
		saveImageListAs();
	}
	else if (!_wpChanger.saveList(_currentListFileName))
	{
		QMessageBox::information(this, "Error saving image list!", "Error occurred while trying to save image list. Try saving to another file or folder.");
	}
	else
		_bListSaved = true;
}

void MainWindow::loadImageList()
{
	if (!_bListSaved)
		promptToSaveList();

	const QString filename = QFileDialog::getOpenFileName(this, "Choose the file to load image list from", QString(), "WallpaperChanger image list file (*.wil)");
	if (!filename.isEmpty())
	{
		setStatusBarMessage("");
		if (!_wpChanger.loadList(filename))
		{
			QMessageBox::information(this, "Error loading image list", "Error occurred while trying to load image list. The file is either inaccessible or corrupt.");
		}
		else
		{
			_currentListFileName = filename;
			_bListSaved = true;
			_previousListSize = _wpChanger.numImages();
			CSettings().setValue(SETTINGS_IMAGE_LIST_FILE, _currentListFileName);
		}
	}
}

// Switching
void MainWindow::nextWallpaper()
{
	_wpChanger.nextWallpaper();
}

void MainWindow::previousWallpaper()
{
	_wpChanger.previousWallpaper();
}

void MainWindow::stopSwitching()
{
	_wpChanger.stopSwitching();
}

void MainWindow::resumeSwitching()
{
	_wpChanger.startSwitching();
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::MiddleClick || reason == QSystemTrayIcon::Trigger)
		restoreWindow();
	else if (reason == QSystemTrayIcon::Context)
	{
		QMenu menu;
		if (_wpChanger.currentWallpaper() != size_t_max)
		{
			menu.addAction(_wpChanger.image(_wpChanger.currentWallpaper()).imageFileName());
			menu.addSeparator();
		}
		menu.addAction(ui->actionNext_wallpaper);
		menu.addAction(ui->actionPrevious_Wallpaper);
		menu.addSeparator();
		menu.addAction(ui->actionDelete_Current_Wallpaper_From_Disk);
		menu.addSeparator();
		menu.addAction(ui->actionExit);

		menu.exec(_trayIcon.geometry().topLeft());
	}
}

void MainWindow::restoreWindow()
{
	show();
	setWindowState( (windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
	raise();
	activateWindow();
}

void MainWindow::trayIconMenu()
{

}

//Key pressed
void MainWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->type() == QKeyEvent::KeyPress && ui->_imageList->hasFocus())
	{
		switch (e->key())
		{
		case Qt::Key_Delete: removeSelectedImages();
			break;
		default:
			QMainWindow::keyPressEvent(e);
		}
	}
}

void MainWindow::selectImage(size_t index)
{
	for (int i = 0; i < ui->_imageList->topLevelItemCount(); ++i)
		if (ui->_imageList->topLevelItem(i)->data(0, Qt::UserRole).toUInt() == index)
		{
			ui->_imageList->topLevelItem(i)->setSelected(true);
		}
}

void MainWindow::removeSelectedImages()
{
	QList<QTreeWidgetItem*> selected = ui->_imageList->selectedItems();
	const int numSelected = selected.size();

	std::vector<size_t> indexesToRemove;
	for (int i = 0; i < numSelected; ++i)
	{
		indexesToRemove.push_back(selected[i]->data(0, Qt::UserRole).toUInt());
	}

	_wpChanger.removeImages(indexesToRemove);
}

//Delete selected images from disk (and from the list if successful)
void MainWindow::deleteSelectedImagesFromDisk()
{
	QList<QTreeWidgetItem*> selected = ui->_imageList->selectedItems();
	const int numSelected = selected.size();

	std::vector<size_t> indexesToRemove;
	for (int i = 0; i < numSelected; ++i)
	{
		indexesToRemove.push_back(selected[i]->data(0, Qt::UserRole).toUInt());
	}

	 _wpChanger.deleteImagesFromDisk(indexesToRemove);
}

void MainWindow::imageListChanged(size_t /*index*/)
{
	updateImageList();
}

void MainWindow::wallpaperChanged(size_t index)
{
	CSettings().setValue(SETTINGS_CURRENT_WALLPAPER, (uint)index);
	for (int i = 0; i < ui->_imageList->topLevelItemCount(); ++i)
	{
		QtImageListItem * item = dynamic_cast<QtImageListItem*>(ui->_imageList->topLevelItem(i));
		if (!item)
			continue;

		if (index < size_t_max && _wpChanger.image(index).id() == (size_t)item->data(0, IdRole).toUInt())
		{
			item->setCurrent();
		}
		else
			item->setCurrent(false);
	}

	ui->_imageList->resizeColumnToContents(MarkerColumn);
}

void MainWindow::timeToNextSwitch(size_t seconds)
{
	_timeToSwitch = seconds;
	const int sec = seconds % 60;
	seconds -= sec;
	const int min = ( seconds / 60 ) % 60;
	seconds -= min * 60;
	const int hr = int (seconds / 3600);
	_statusBarTimeToSwitchLabel.setText(QString("%1:%2:%3").arg(hr, 2, 10, QChar('0') ).arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0')));
}

// UI setup
void MainWindow::initToolbar()
{
	connect (ui->actionPrevious_Wallpaper, SIGNAL(triggered()), SLOT(previousWallpaper()));
	connect (ui->actionNext_wallpaper, SIGNAL(triggered()), SLOT(nextWallpaper()));
	connect (ui->actionStop_Switching, SIGNAL(triggered()), SLOT(stopSwitching()));
	connect (ui->actionStart_switching, SIGNAL(triggered()), SLOT(resumeSwitching()));

	connect (ui->actionRemove_Current_Image_from_List, SIGNAL(triggered()), SLOT(removeCurrentWp()));
	connect (ui->actionDelete_Current_Wallpaper_From_Disk, SIGNAL(triggered()), SLOT(deleteCurrentWp()));

	ui->actionStart_switching->setChecked(CSettings().value(SETTINGS_START_SWITCHING_ON_STARTUP, SETTINGS_DEFAULT_AUTOSTART).toBool());
}
