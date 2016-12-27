#include "wallpaperchanger.h"
#include "settings.h"
#include "settings/csettings.h"

DISABLE_COMPILER_WARNINGS
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStringList>
RESTORE_COMPILER_WARNINGS

#include <algorithm>
#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#pragma comment(lib, "user32.lib")
#endif

#define TIMER_INTERVAL 1000

WallpaperChanger::WallpaperChanger():
	_currentWPId(invalid_id),
	_bUpdatesEnabled(true)
{
	_qTimer.setInterval(TIMER_INTERVAL);
	_qTimer.setSingleShot(false);
	QObject::connect(&_qTimer, &QTimer::timeout, [this]() {
		onTimeout();
	});

	srand((unsigned int)time(nullptr));

	invokeCallback(&WallpaperWatcher::timeToNextSwitch, interval() - 1);

	_imageList.addSubscriber(this);
}

WallpaperChanger& WallpaperChanger::instance()
{
	static WallpaperChanger inst;
	return inst;
}


size_t WallpaperChanger::indexByID(qulonglong id) const
{
	assert_and_return_r(_indexById.count(id) > 0, invalid_index);
	return _indexById.at(id);
}

qulonglong WallpaperChanger::idByIndex(size_t index) const
{
	for (auto it = _indexById.begin(); it != _indexById.end(); ++it)
	{
		if (it->second == index)
			return it->first;
	}

	return invalid_index;

}

void WallpaperChanger::setCurrentWpIndex(size_t index)
{
	if (index < numImages())
		setWallpaper(index);
	else
		_currentWPId = invalid_id;
}

bool WallpaperChanger::addImage(const QString &filename, ImgParams params /* = ImgParams() */)
{
	Image img (filename, params);
	if (img.isValidImage ())
	{
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

	if (succ)
	{
		// Reset timer
		_qTime.restart();
		_qTime.start();

		_currentWPId = _imageList[idx].id();
		if (addToHistory)
			_previousWallPapers.addLatest(_currentWPId);
	}
	else
		qDebug() << "Failed to set wallpaper " << normalizeFileName(image(idx).imageFilePath());

	return succ;
}

// Delete images from disk by IDs
void WallpaperChanger::deleteImagesFromDisk(const std::vector<qulonglong> &batchIDs)
{
	std::vector<size_t> batchIndexes;
	for (auto id = batchIDs.begin(); id != batchIDs.end(); ++id)
	{
		auto index = _indexById.find(*id);
		if (index != _indexById.end())
			batchIndexes.push_back(index->second);
	}

	_imageList.deleteFilesFromDisk(batchIndexes);
	adjustHistoryForObsoleteImages();
	if (std::find(batchIDs.begin(), batchIDs.end(), _currentWPId) != batchIDs.end())
	{
		_currentWPId = invalid_id;
		invokeCallback(&WallpaperWatcher::wallpaperChanged, invalid_index);
	}
}

// Remove batch of images from the list by their IDs
void WallpaperChanger::removeImages(const std::vector<qulonglong> &batchIDs)
{
	std::vector<size_t> batchIndexes;
	for (auto id = batchIDs.begin(); id != batchIDs.end(); ++id)
	{
		auto index = _indexById.find(*id);
		if (index != _indexById.end())
			batchIndexes.push_back(index->second);
	}

	_imageList.removeImages(batchIndexes);
	adjustHistoryForObsoleteImages();
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
	if (_qTimer.isActive())
		startSwitching();

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
	return _indexById.count(_currentWPId) > 0 ? _indexById.at(_currentWPId) : invalid_index;
}

bool WallpaperChanger::stopped() const
{
	return _qTimer.isActive();
}

void WallpaperChanger::enableListUpdateCallbacks(bool enable /* = true*/)
{
	_bUpdatesEnabled = enable;
}

// Signal that image list has changed
void WallpaperChanger::listChanged(size_t /*index*/)
{
	_currentWPId = invalid_id;
	adjustHistoryForObsoleteImages();
	for (size_t index = 0; index < _imageList.size(); ++index)
	{
		_indexById[_imageList[index].id()] = index;
	}

	if (_bUpdatesEnabled)
		invokeCallback(&WallpaperWatcher::listChanged, invalid_index);
}

// Signal that image list has been cleared
void WallpaperChanger::listCleared()
{
	_indexById.clear();
	_currentWPId = invalid_id;

	if (_bUpdatesEnabled)
		invokeCallback(&WallpaperWatcher::listCleared);
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

	invokeCallback(&WallpaperWatcher::timeToNextSwitch, t);
}

// Sets the image as a wallpaper
bool WallpaperChanger::setWallpaperImpl(size_t idx)
{
	const QString normPath = normalizeFileName(image(idx).imageFilePath());

#ifdef _WIN32
	{
		QSettings settings("HKEY_CURRENT_USER\\Control Panel\\Desktop", QSettings::NativeFormat);
		if (settings.value("TileWallpaper", "0") != "0")
		{
			Sleep(100);
			settings.setValue("TileWallpaper", "0");
		}
		settings.setValue("WallpaperStyle", image(idx).stretchMode() == CENTERED ? "0" : "6");
	}

	const BOOL succ =  SystemParametersInfoW(SPI_SETDESKWALLPAPER, 1, (void*)normPath.utf16(), SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
	if (succ)
		invokeCallback(&WallpaperWatcher::wallpaperChanged, idx);

	return succ != 0;
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
void WallpaperChanger::adjustHistoryForObsoleteImages()
{
	decltype(_previousWallPapers) newHistory;
	for (size_t i = 0; i < _previousWallPapers.size(); ++i)
	{
		// If image is no longer present - delete it from history
		for (size_t imageIndex = 0; imageIndex < _imageList.size(); ++imageIndex)
			if (_imageList[imageIndex].id() == _previousWallPapers[i])
			{
				newHistory.addLatest(_previousWallPapers[i]);
				break;
			}
	}

	_previousWallPapers = newHistory;
}

QString WallpaperChanger::normalizeFileName( QString filename )
{
#ifdef _WIN32
	return filename.replace('/', "\\");
#else
	return filename;
#endif
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

	if (_previousWallPapers.empty() || _previousWallPapers.isAtEnd())
	{
		// Choosing new wallpaper
		const bool randomize = CSettings().value(SETTINGS_RANDOMIZE, SETTINGS_DEFAULT_RANDOMIZE).toBool();
		size_t newWpIndex = invalid_index;
		if (randomize)
		{
			newWpIndex = (((size_t)rand() << 16) | rand()) % _imageList.size();
		}
		else
		{
			size_t currentWpIndex = indexByID(_currentWPId);
			newWpIndex = currentWpIndex < _imageList.size() - 1 ? currentWpIndex + 1 : 0;
		}
		_previousWallPapers.addLatest(_imageList[newWpIndex].id());
		return setWallpaper(newWpIndex, false);
	}
	else
	{
		// Navigating through previously set wallpapers
		_currentWPId = _previousWallPapers.navigateForward();
		return setWallpaper(indexByID(_currentWPId), false);
	}
}

void WallpaperChanger::previousWallpaper()
{
	if (!_previousWallPapers.empty() && !_previousWallPapers.isAtBeginning())
	{
		_currentWPId = _previousWallPapers.navigateBack();
		setWallpaper(indexByID(_currentWPId), false);
	}
}
