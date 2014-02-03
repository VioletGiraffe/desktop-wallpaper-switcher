#ifndef IMAGELIST_H
#define IMAGELIST_H

#include "image.h"
#include "signals/signal.h"

#include <vector>
#include <limits>

const size_t size_t_max = std::numeric_limits<size_t>().max();

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

	void enableUpdates (bool enable = true);

	//Deletes corresponding files from disk and removes from the list if deletion successful
	bool deleteFilesFromDisk (const std::vector<size_t>& indexes);

	bool saveList (const QString& filename) const;
	bool loadList (const QString& filename);

	Signal<size_t /*index*/> _signalListChanged;

private:
	std::vector<Image> _list;
	bool               _bUpdatesEnabled;
};

#endif // IMAGELIST_H
