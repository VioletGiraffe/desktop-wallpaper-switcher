#pragma once

#include "image.h"
#include "utility/callback_caller.hpp"

#include <limits>
#include <vector>

const size_t invalid_index = std::numeric_limits<size_t>().max();
const qulonglong invalid_id = std::numeric_limits<qulonglong>().max();

struct ImageListWatcher {
	virtual void listCleared() = 0;
	virtual void listChanged(size_t index) = 0;
};

class ImageList : public CallbackCaller<ImageListWatcher>
{
public:
	ImageList();

	size_t size () const;
	void addImage (const Image& image);
	void removeImages (const std::vector<size_t>& indexes);
	void clear ();
	bool empty () const;
	Image& operator[] (size_t index);
	const Image& operator[] (size_t index) const;

	// Deletes corresponding files from disk and removes from the list if deletion successful
	bool deleteFilesFromDisk (const std::vector<size_t>& indexes);

	bool saveList (const QString& filename) const;
	bool loadList (const QString& filename);

private:
	std::vector<Image> _list;
};
