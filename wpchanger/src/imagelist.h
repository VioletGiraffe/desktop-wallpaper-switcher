#ifndef IMAGELIST_H
#define IMAGELIST_H

#include "image.h"
#include "signals/signal.h"

#include <vector>
#include <limits>

const size_t invalid_index = std::numeric_limits<size_t>().max();
const qulonglong invalid_id = std::numeric_limits<qulonglong>().max();

class ImageList
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

	//Deletes corresponding files from disk and removes from the list if deletion successful
	bool deleteFilesFromDisk (const std::vector<size_t>& indexes);

	bool saveList (const QString& filename) const;
	bool loadList (const QString& filename);

	Signal0                   _signalListCleared;
	Signal1<size_t /*index*/> _signalListChanged;

private:
	std::vector<Image> _list;
};

#endif // IMAGELIST_H
