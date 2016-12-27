#ifndef WALLPAPERCHANGER_H
#define WALLPAPERCHANGER_H

#include "compiler/compiler_warnings_control.h"

#include "imagelist.h"
#include "historylist/chistorylist.h"

DISABLE_COMPILER_WARNINGS
#include <QString>
#include <QTime>
#include <QTimer>
RESTORE_COMPILER_WARNINGS

#include <map>

struct WallpaperWatcher {
	virtual void wallpaperChanged(size_t) = 0;
	virtual void wallpaperAdded(size_t) = 0;
	virtual void timeToNextSwitch(size_t) = 0;
	virtual void listChanged(size_t) = 0;
	virtual void listCleared() = 0;
};

class WallpaperChanger : public ImageListWatcher, public CallbackCaller<WallpaperWatcher>
{
private:
	WallpaperChanger();

public:
	static WallpaperChanger& instance ();

// Wallpapers
	size_t indexByID(qulonglong id) const;
	qulonglong idByIndex(size_t index) const;

	void setCurrentWpIndex(size_t index);

	// Adds the file to image list
	bool addImage(const QString& filename, ImgParams params = ImgParams());
	// Returns QImage by its index in the list
	QImage createQImage(size_t idx) const;
	// Returns Image by its index in the list
	const Image &image(size_t idx) const;
	// Sets the image as a wallpaper
	bool setWallpaper(size_t idx, bool addToHistory = true);
	// Delete images from disk by IDs
	void deleteImagesFromDisk(const std::vector<qulonglong /*ids*/>& batch);
	// Remove batch of images from the list by their IDs
	void removeImages(const std::vector<qulonglong /*ids*/>& batch);
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

	bool stopped() const;

// Notifications
	void enableListUpdateCallbacks(bool enable = true);

// Listeners
	// Signal that image list has changed
	void listChanged(size_t index) override;
	// Signal that image list has been cleared
	void listCleared() override;

private:
	void onTimeout();

	// Sets the image as a wallpaper
	bool setWallpaperImpl (size_t idx);

	// Check if any of the images from a list provided are in history, adjust history if so (to prevent invalid history record)
	void adjustHistoryForObsoleteImages ();

private:
	ImageList    _imageList;
	qulonglong   _currentWPId;
	std::map<qulonglong /*id*/, size_t /*index*/> _indexById;
	bool         _bUpdatesEnabled;

// Time
	// List of previously active wallpapers for back/forth navigation
	CHistoryList<qulonglong> _previousWallPapers;
	QTimer                   _qTimer;
	// Time since last switch
	QTime                    _qTime;

private:
	static QString normalizeFileName(QString filename);
};

#endif // WALLPAPERCHANGER_H
