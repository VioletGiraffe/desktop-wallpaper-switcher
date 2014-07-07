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
	_currentWPId(invalid_id),
	_bUpdatesEnabled(true)
{
	_qTimer.setInterval(TIMER_INTERVAL);
	_qTimer.setSingleShot(false);
	connect(&_qTimer, SIGNAL(timeout()), SLOT(onTimeout()));

	srand(time(0));

	_signalTimeToNextSwitch.invoke(interval() - 1);

	_slotListChanged = _imageList._signalListChanged.connect(this, &WallpaperChanger::listChanged);
	_slotListCleared = _imageList._signalListCleared.connect(this, &WallpaperChanger::listCleared);
}

WallpaperChanger& WallpaperChanger::instance()
{
	static WallpaperChanger inst;
	return inst;
}


size_t WallpaperChanger::indexByID(qulonglong id) const
{
	if (_indexById.count(id) > 0)
		return _indexById.at(id);
	else
	{
		assert(_indexById.count(id) > 0);
		return invalid_index;
	}
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
		_signalWallpaperChanged.invoke(invalid_index);
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

// Signal that image list has changed
void WallpaperChanger::listChanged(size_t /*index*/ /* = size_t_max*/)
{
	_currentWPId = invalid_id;
	adjustHistoryForObsoleteImages();
	for (size_t index = 0; index < _imageList.size(); ++index)
	{
		_indexById[_imageList[index].id()] = index;
	}

	if (_bUpdatesEnabled)
		_signalListChanged.invoke(invalid_index);
}

// Signal that image list has been cleared
void WallpaperChanger::listCleared()
{
	_indexById.clear();
	_currentWPId = invalid_id;

	if (_bUpdatesEnabled)
		_signalListCleared.invoke();
}

void WallpaperChanger::enableListUpdateCallbacks(bool enable /* = true*/)
{
	_bUpdatesEnabled = enable;
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
			Sleep(100);
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
