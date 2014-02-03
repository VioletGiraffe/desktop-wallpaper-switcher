#include "imagebrowserwindow.h"
#include "imagelist.h"
#include "wallpaperchanger.h"
#include "ui_imagebrowserwindow.h"
#include "time/ctime.h"

#include <QMessageBox>
#include <QDebug>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QListWidgetItem>
#include <QDesktopServices>
#include <QUrl>

#include <cstdlib>
#include <omp.h>

static const QSize maxThumbSize (300, 300);


ImageBrowserWindow::ImageBrowserWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::ImageBrowserWindow),
	_wpChanger(WallpaperChanger::instance())
{
	ui->setupUi(this);
	ui->_thumbnailBrowser->setIconSize(QSize(200,200));

	_zoomInShortcut = new(std::nothrow) QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus), this, SLOT(zoomIn()));
	_zoomOutShortcut = new(std::nothrow) QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus), this, SLOT(zoomOut()));

	connect(ui->_thumbnailBrowser, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(showContextMenu(const QPoint&)));
	connect(ui->_thumbnailBrowser, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(itemActivated(QListWidgetItem*)));
}

ImageBrowserWindow::~ImageBrowserWindow()
{
	delete ui;
}

bool ImageBrowserWindow::populate()
{
	CTime start;
	ui->_thumbnailBrowser->clear();

	QListWidgetItem ** items = new QListWidgetItem* [_wpChanger.numImages()];
	memset(items, 0, _wpChanger.numImages() * sizeof(QListWidgetItem*));

	//omp_set_num_threads(omp_get_num_threads() > 4 ? 4 : omp_get_num_threads());
#pragma omp parallel for schedule(static,50)
	for (int i = 0; i < (int)_wpChanger.numImages(); ++i)
	{
		if (_wpChanger.imageExists(i))
		{
			items[i] = new QListWidgetItem(QIcon(QPixmap::fromImage(_wpChanger.image(i).constructQImageObject().scaled(maxThumbSize, Qt::KeepAspectRatio, Qt::SmoothTransformation))),
										   _wpChanger.image(i).imageFileName() + QString (" (%1x%2)").arg(_wpChanger.image(i).params()._width).arg(_wpChanger.image(i).params()._height));
			items[i]->setData(Qt::UserRole, _wpChanger.image(i).imageFilePath());
		}
	}

	for (size_t i = 0; i < _wpChanger.numImages(); ++i)
	{
		if (items[i])
			ui->_thumbnailBrowser->addItem(items[i]);
	}

	delete items;

	qDebug() << "Populating thumb explorer with " << _wpChanger.numImages() << " items took " << (CTime() - start)/1000 << " s";
	return true;
}

bool ImageBrowserWindow::event(QEvent * event)
{
	if (event->type() == QEvent::Show)
	{
		populate();
		raise();
		activateWindow();
	}

	return QMainWindow::event(event);
}

void ImageBrowserWindow::showContextMenu(const QPoint &pos)
{
	const QPoint globalPos = ui->_thumbnailBrowser->viewport()->mapToGlobal(pos);

	QMenu menu;
	QAction * setAsWallpaper = menu.addAction("Set as wallpaper");
	menu.addSeparator();
	QAction * deleteFromDisk = menu.addAction("Delete from disk");

	QAction * selectedItem = menu.exec(globalPos);
	if (selectedItem == deleteFromDisk)
	{
		if (QMessageBox::question(this, "Are you sure?", "Are you sure you want to irreversibly delete the selected image(s) from disk?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
			deleteSelectedImagesFromDisk();
	}
	else if (selectedItem == setAsWallpaper && ui->_thumbnailBrowser->selectedItems().size() > 0)
		_wpChanger.setWallpaper(imageIndexByPath(ui->_thumbnailBrowser->selectedItems().at(0)->data(Qt::UserRole).toString()));
}

// Image double-clicked
void ImageBrowserWindow::itemActivated(QListWidgetItem * item)
{
	if (item)
	{
		QDesktopServices::openUrl(QUrl::fromLocalFile(_wpChanger.image(imageIndexByPath(item->data(Qt::UserRole).toString())).imageFilePath()));
	}
}

void ImageBrowserWindow::zoomIn()
{
	const QSize newSize = ui->_thumbnailBrowser->iconSize() + QSize(50, 50);
	if (newSize.width() <= maxThumbSize.width() && newSize.height() <= maxThumbSize.height())
		ui->_thumbnailBrowser->setIconSize(newSize);
}

void ImageBrowserWindow::zoomOut()
{
	const QSize newSize = ui->_thumbnailBrowser->iconSize() - QSize(50, 50);
	if (newSize.width() > 0 && newSize.height() > 0)
		ui->_thumbnailBrowser->setIconSize(newSize);
}

void ImageBrowserWindow::removeSelectedImages()
{
	const QList<QListWidgetItem*> selected = ui->_thumbnailBrowser->selectedItems();
	const int numSelected = selected.size();

	std::vector<size_t> indexesToRemove;
	for (int i = 0; i < numSelected; ++i)
	{
		indexesToRemove.push_back(imageIndexByPath(selected[i]->data(Qt::UserRole).toString()));
		removeItemFromView(selected[i]);
	}

	_wpChanger.removeImages(indexesToRemove);
}

//Delete selected images from disk (and from the list if successful)
void ImageBrowserWindow::deleteSelectedImagesFromDisk()
{
	const QList<QListWidgetItem*> selected = ui->_thumbnailBrowser->selectedItems();
	const int numSelected = selected.size();

	std::vector<size_t> indexesToRemove;
	for (int i = 0; i < numSelected; ++i)
	{
		indexesToRemove.push_back(imageIndexByPath(selected[i]->data(Qt::UserRole).toString()));
		removeItemFromView(selected[i]);
	}


	_wpChanger.deleteImagesFromDisk(indexesToRemove);
}

// Will also delete item!
void ImageBrowserWindow::removeItemFromView( QListWidgetItem * item )
{
	if (item != 0)
	{
		ui->_thumbnailBrowser->takeItem(ui->_thumbnailBrowser->row(item));
		delete item;
	}
}

// Finds image index in the image list by its file path
size_t ImageBrowserWindow::imageIndexByPath(QString path) const
{
	for (size_t i = 0; i < _wpChanger.numImages(); ++i)
		if (_wpChanger.image(i).imageFilePath() == path)
			return i;

	return -1;
}
