#include "imagebrowserwindow.h"
#include "imagelist.h"
#include "wallpaperchanger.h"
#include "system/ctimeelapsed.h"

DISABLE_COMPILER_WARNINGS
#include "ui_imagebrowserwindow.h"

#include <QMessageBox>
#include <QDebug>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QListWidgetItem>
#include <QDesktopServices>
#include <QShortcut>
#include <QUrl>
RESTORE_COMPILER_WARNINGS

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

	QShortcut* zoomInShortcut = new(std::nothrow) QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus), this);
	connect(zoomInShortcut, &QShortcut::activated, this, &ImageBrowserWindow::zoomIn);

	QShortcut* zoomOutShortcut = new(std::nothrow) QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus), this);
	connect(zoomOutShortcut, &QShortcut::activated, this, &ImageBrowserWindow::zoomOut);

	connect(ui->_thumbnailBrowser, &QListWidget::customContextMenuRequested, this, &ImageBrowserWindow::showContextMenu);
	connect(ui->_thumbnailBrowser, &QListWidget::itemActivated, this, &ImageBrowserWindow::itemActivated);
}

ImageBrowserWindow::~ImageBrowserWindow()
{
	delete ui;
}

bool ImageBrowserWindow::populate()
{
	CTimeElapsed stopWatch(true);

	ui->_thumbnailBrowser->clear();

	std::vector<QListWidgetItem*> items(_wpChanger.numImages(), nullptr);

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

	qDebug() << "Creating" << items.size() << "items took" << stopWatch.elapsed() / 1000.0f << "seconds";
	stopWatch.start();

	for (QListWidgetItem* item: items)
		ui->_thumbnailBrowser->addItem(item);

	qDebug() << "Populating explorer with" << items.size() << "thumbnails took" << stopWatch.elapsed<std::chrono::seconds>() << "seconds";
	return true;
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
		_wpChanger.setWallpaper(_wpChanger.indexByID(imageIdByPath(ui->_thumbnailBrowser->selectedItems().at(0)->data(Qt::UserRole).toString())));
}

// Image double-clicked
void ImageBrowserWindow::itemActivated(QListWidgetItem * item)
{
	if (item)
		QDesktopServices::openUrl(QUrl::fromLocalFile(_wpChanger.image(_wpChanger.indexByID(imageIdByPath(item->data(Qt::UserRole).toString()))).imageFilePath()));
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

//Delete selected images from disk (and from the list if successful)
void ImageBrowserWindow::deleteSelectedImagesFromDisk()
{
	const QList<QListWidgetItem*> selected = ui->_thumbnailBrowser->selectedItems();

	std::vector<qulonglong> idsToRemove;
	for (int i = 0; i < selected.size(); ++i)
	{
		idsToRemove.push_back(imageIdByPath(selected[i]->data(Qt::UserRole).toString()));
		removeItemFromView(selected[i]);
	}


	_wpChanger.deleteImagesFromDisk(idsToRemove);
}

void ImageBrowserWindow::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);

	populate();
	raise();
	activateWindow();
}

void ImageBrowserWindow::closeEvent(QCloseEvent *event)
{
	QMainWindow::closeEvent(event);

	ui->_thumbnailBrowser->clear();
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
qulonglong ImageBrowserWindow::imageIdByPath(const QString& path) const
{
	for (size_t i = 0; i < _wpChanger.numImages(); ++i)
		if (_wpChanger.image(i).imageFilePath() == path)
			return _wpChanger.image(i).id();

	return std::numeric_limits<qulonglong>::max();
}
