#ifndef IMAGELISTITEM_H
#define IMAGELISTITEM_H

#include "image.h"
#include <QTreeWidgetItem>

const int IdRole = Qt::UserRole + 1;

// Columns
enum {
	MarkerColumn,
	FileNameColumn,
	BppColumn,
	DimensionsColumn,
	FileSizeColumn,
	DisplayModeColumn,
	ImageFormatColumn,
	FolderColumn
};

class QtImageListItem : public QTreeWidgetItem
{
public:
	QtImageListItem(const Image& img, size_t index, bool currentlySetWallpaper = false);

	virtual bool operator<(const QTreeWidgetItem &other) const;

	void setCurrent(bool current = true);
};

#endif // IMAGELISTITEM_H
