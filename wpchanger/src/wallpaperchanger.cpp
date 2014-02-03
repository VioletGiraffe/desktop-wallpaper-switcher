#include "wallpaperchanger.h"
#include "settings.h"
#include "settings/csettings.h"

#include <QCoreApplication>
#include <QFile>
#include <QStringList>
#include <QFileInfo>
#include <QSettings>
#include <QDebug>

#include <algorithm>
#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#pragma comment(lib, "user32.lib")
#endif

#include <assert.h>

#define TIMER_INTERVAL 1000

WallpaperChanger::WallpaperChanger():
	_currentWPIdx (size_t_max),
	_currentWPIdxInNavigationList(0)
{
	_qTimer.setInterval(TIMER_INTERVAL);
	_qTimer.setSingleShot(false);
	connect(&_qTimer, SIGNAL(timeout()), SLOT(onTimeout()));

	srand(time(0));

	_signalTimeToNextSwitch.invoke(interval() - 1);

	_slotListChanged = _imageList._signalListChanged.connect(this, &WallpaperChanger::listChanged);

	if (CSettings().value(SETTINGS_START_SWITCHING_ON_STARTUP, SETTINGS_DEFAULT_AUTOSTART).toBool())
		startSwitching();
}

WallpaperChanger&  WallpaperChanger::instance()
{
	static WallpaperChanger inst;
	return inst;
}

bool WallpaperChanger::addImage(const QString &filename, ImgParams params /* = ImgParams() */)
{
	Image img (filename, params);
	if (img.isValidImage ())
	{
		_previousWallPapers.clear();
		_imageList.addImage(img);
		return true;
	}
	else
		return false;
}

QImage WallpaperChanger::createQImage( size_t idx ) const
{
	if (idx < _imageList.size ())
		return _imageList[idx].constructQImageObject();
	else
		return QImage();
}

const Image& WallpaperChanger::image( size_t idx ) const
{
	return _imageList[idx];
}

bool WallpaperChanger::setWallpaper( size_t idx, bool addToHistory /*= true*/ )
{
	const bool succ = setWallpaperImpl(idx);
	if ( succ && addToHistory )
		_previousWallPapers.push_back(idx);

	if (!succ)
		qDebug() << "Failed to set wallpaper " << normalizeFileName(image(idx).imageFilePath());
	return succ;
}

// Delete image from disk
void WallpaperChanger::deleteImagesFromDisk(const std::vector<size_t> &batch)
{
	adjustHistoryForObsoleteImages(batch);
	_imageList.deleteFilesFromDisk(batch);
	if (std::find(batch.begin(), batch.end(), _currentWPIdx) != batch.end())
	{
		_currentWPIdx = size_t_max;
		_signalWallpaperChanged.invoke(size_t_max);
	}
}

// Remove batch of images from the list
void WallpaperChanger::removeImages(const std::vector<size_t> &batch)
{
	adjustHistoryForObsoleteImages(batch);
	_imageList.removeImages(batch);
}

// Remove non-existent entries from list
void WallpaperChanger::removeNonexistentEntries()
{
	std::vector<size_t> toRemove;
	for (size_t i = 0; i < numImages(); ++i)
		if (!imageExists(i))
			toRemove.push_back(i);

	_imageList.removeImages(toRemove);
}

size_t WallpaperChanger::numImages() const
{
	return _imageList.size();
}

// Returns true if image physically exists on disk
bool WallpaperChanger::imageExists(size_t index) const
{
	return index < numImages() && QFileInfo(_imageList[index].imageFilePath()).exists();
}

bool WallpaperChanger::saveList(const QString &filename) const
{
	return _imageList.saveList(filename);
}

bool WallpaperChanger::loadList(const QString &filename)
{
	return _imageList.loadList(filename);
}

void WallpaperChanger::setInterval(int seconds)
{
	CSettings().setValue(SETTINGS_INTERVAL, seconds);
}


int WallpaperChanger::interval()
{
	return CSettings().value(SETTINGS_INTERVAL, SETTINGS_DEFAULT_INTERVAL).toInt();
}


int WallpaperChanger::timeLeft() const
{
	const int t = interval() - _qTime.elapsed() / 1000;
	return t >= 0 ? t : 0;
}

// Index of the currently set wallpaper (size_t_max if none from the list is set)
size_t WallpaperChanger::currentWallpaper() const
{
	return _currentWPIdx;
}

// Signal that image list has changed
void WallpaperChanger::listChanged(size_t /*index*/ /* = size_t_max*/)
{
	_signalListChanged.invoke(size_t_max);
}


void WallpaperChanger::enableListUpdateCallbacks(bool enable /* = true*/)
{
	_imageList.enableUpdates(enable);
}

void WallpaperChanger::onTimeout()
{
	int t = interval() - _qTime.elapsed() / 1000;
	if (t <= 0)
	{
		_qTime.restart();
		t = 0;
		while (!nextWallpaper());
	}

	_signalTimeToNextSwitch.invoke(t);
}

// Sets the image as a wallpaper
bool WallpaperChanger::setWallpaperImpl(size_t idx)
{
	QString normPath = normalizeFileName(image(idx).imageFilePath());

#ifdef _WIN32
	{
		QSettings settings("HKEY_CURRENT_USER\\Control Panel\\Desktop", QSettings::NativeFormat);
		if (settings.value("TileWallpaper", "0") != "0")
		{
			Sleep(300);
			settings.setValue("TileWallpaper", "0");
		}
		settings.setValue("WallpaperStyle", image(idx).stretchMode() == CENTERED ? "0" : "6");
	}
	BOOL succ =  SystemParametersInfoW(SPI_SETDESKWALLPAPER, 1, (void*)normPath.utf16(), SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
	if (succ)
	{
		_signalWallpaperChanged.invoke(idx);
	}
	return succ;
#else
#pragma warning "Setting wallpaper is not implemented for this platform"
#if defined __linux__
#elif defined __APPLE__
#else
#error unkown platform
#endif
#endif
}

// Check if any of the images from a list provided are in history, adjust history if so (to prevent invalid history record)
void WallpaperChanger::adjustHistoryForObsoleteImages(const std::vector<size_t> &batch)
{
	decltype(_previousWallPapers) newHistory;
	for (auto it = _previousWallPapers.begin(); it != _previousWallPapers.end(); ++it)
	{
		// If image being removed is in history - delete it from history
		if (std::find(batch.begin(), batch.end(), *it) == batch.end())
			newHistory.push_back(*it);
	}

	_previousWallPapers = newHistory;
}

QString WallpaperChanger::normalizeFileName( const QString filename )
{
	QString normalizedName (filename);
	#ifdef _WIN32
	normalizedName.replace('/', "\\");
	#endif
	return normalizedName;
}

bool WallpaperChanger::isSupportedImageFile( const QString& file )
{
	bool supported = false;
	QString fileToLower (file.toLower());
	if (fileToLower.contains(".jpg")  ||
		fileToLower.contains(".jpeg") ||
		fileToLower.contains(".bmp")  ||
		fileToLower.contains(".png")  ||
		fileToLower.contains(".gif") ||
		fileToLower.contains(".tif")  ||
		fileToLower.contains(".tiff")
		)
	{
		supported = true;
	}

	return supported;
}

//Switching
void WallpaperChanger::startSwitching ()
{
	_qTime.restart();
	_qTime.start();
	_qTimer.start();
}

void WallpaperChanger::stopSwitching ()
{
	_qTimer.stop();
}

bool WallpaperChanger::nextWallpaper()
{
	if (numImages() <= 0)
		return true;

	if (_currentWPIdxInNavigationList >= _previousWallPapers.size() - 1 || _previousWallPapers.empty())
	{
		// Choosing new wallpaper
		const bool randomize = CSettings().value(SETTINGS_RANDOMIZE, SETTINGS_DEFAULT_RANDOMIZE).toBool();
		if (randomize)
		{
			_currentWPIdx = ( ((size_t)rand() << 16) | rand() ) % _imageList.size();
		}
		else
		{
			_currentWPIdx = _currentWPIdx < _imageList.size() - 1 ? _currentWPIdx + 1 : 0;
		}
		_previousWallPapers.push_back(_currentWPIdx);
		_currentWPIdxInNavigationList = _previousWallPapers.size() - 1;
	}
	else
	{
		// Navigating through previously set wallpapers
		_currentWPIdx = _previousWallPapers[++_currentWPIdxInNavigationList];
	}

	return setWallpaper(_currentWPIdx, false);
}

void WallpaperChanger::previousWallpaper  ()
{
	if (!_previousWallPapers.empty() && _currentWPIdxInNavigationList < _previousWallPapers.size() && (_currentWPIdxInNavigationList - 1) < _previousWallPapers.size())
	{
		_currentWPIdx = _previousWallPapers[--_currentWPIdxInNavigationList];
		setWallpaper(_currentWPIdx, false);
	}
}
