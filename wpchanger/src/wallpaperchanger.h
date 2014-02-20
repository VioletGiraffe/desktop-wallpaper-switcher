#ifndef WALLPAPERCHANGER_H
#define WALLPAPERCHANGER_H

#include "imagelist.h"
#include "signals/signal.h"

#include <map>
#include <QString>
#include <QTime>
#include <QTimer>

class WallpaperChanger : private QObject
{
	Q_OBJECT

private:
	WallpaperChanger();

public:
	static WallpaperChanger& instance ();

// Wallpapers
	size_t indexByID(size_t id) const;
	size_t idByIndex(size_t index) const;

	void setCurrentWpIndex(size_t index);

	// Adds the file to image list
	bool addImage(const QString& filename, ImgParams params = ImgParams());
	// Returns QImage by it's index in the list
	QImage createQImage(size_t idx) const;
	// Returns Image by it's index in the list
	const Image &image(size_t idx) const;
	// Sets the image as a wallpaper
	bool setWallpaper(size_t idx, bool addToHistory = true);
	// Delete images from disk by IDs
	void deleteImagesFromDisk(const std::vector<size_t /*indexes*/>& batch);
	// Remove batch of images from the list by their IDs
	void removeImages(const std::vector<size_t /*indexes*/>& batch);
	// Remove non-existent entries from list
	void removeNonexistentEntries();

	// Number of images in the list
	size_t numImages() const;
	// Returns true if image physically exists on disk
	bool imageExists(size_t index) const;

	bool saveList(const QString& filename) const;
	bool loadList(const QString& filename);


// Settings
	// Interval between wallpaper switching in seconds
	static void setInterval(int seconds);
	static int  interval();

	static bool isSupportedImageFile(const QString& file);

// Switching
	void startSwitching();
	void stopSwitching();
	bool nextWallpaper();
	void previousWallpaper();
	// Time left to next switch
	int timeLeft() const;
	// Index of the currently set wallpaper (size_t_max if none from the list is set)
	size_t currentWallpaper() const;

// Notifications
	// Signal that image list has changed
	void listChanged(size_t index = invalid_index);
	// Signal that image list has been cleared
	void listCleared();

public /*signals*/:
	void enableListUpdateCallbacks (bool enable = true);
	Signal1<size_t> _signalWallpaperChanged;
	Signal1<size_t> _wallpaperAdded;
	Signal1<size_t> _signalTimeToNextSwitch;
	Signal1<size_t> _signalListChanged;
	Signal0          _signalListCleared;


private slots:
	void onTimeout();

private:
	// Sets the image as a wallpaper
	bool setWallpaperImpl (size_t idx);

	// Check if any of the images from a list provided are in history, adjust history if so (to prevent invalid history record)
	void adjustHistoryForObsoleteImages (const std::vector<size_t>& batch);

private:
	ImageList    _imageList;
	size_t       _currentWPIdx;
	size_t       _currentWPIdxInNavigationList;
	std::map<size_t /*id*/, size_t /*index*/> _indexById;

// Time
	// List of previously active wallpapers for back/forth navigation
	std::vector<size_t> _previousWallPapers;
	QTimer              _qTimer;
	// Time since last switch
	QTime               _qTime;
	Slot                _slotListChanged;
	Slot                _slotListCleared;

private:
	static QString normalizeFileName(const QString filename);
};

#endif // WALLPAPERCHANGER_H
