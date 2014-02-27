#include "qtimagelistitem.h"

#include <QTreeWidgetItem>
#include <assert.h>

static const unsigned char currentWpMarkSymbol[] = {
	0xB6, // ▶
	0x25
};

QtImageListItem::QtImageListItem(const Image& img, bool currentlySetWallpaper/* = false*/)
{
	if (currentlySetWallpaper)
		setText(MarkerColumn, QString::fromUtf16((const ushort*)currentWpMarkSymbol));

	const qulonglong id = img.id();

	setData(0, IdRole, QVariant(id)); // This WP's ID

	setText(FileNameColumn, img.imageFileName());

	const double bpp = img.params()._fileSize * 8 / double (img.params()._width * img.params()._height);
	setText(BppColumn, QString::number(bpp, 'g', 3));

	setText(DimensionsColumn, QString("%1x%2").arg(img.params()._width).arg(img.params()._height));
	setData(DimensionsColumn, Qt::UserRole, img.params()._width * img.params()._height);	// This wallpaper's resolution

	setText(FileSizeColumn, QString("%1").arg(img.params()._fileSize / 1024) + " KB");
	setData(FileSizeColumn, Qt::UserRole, img.params()._fileSize / 1024);	// This wallpaper's file size

	setText(DisplayModeColumn, QString(img.params()._wpDisplayMode == CENTERED ? "Centered" : img.params()._wpDisplayMode == SYSTEM_DEFAULT ? "Default" : "Stretched"));

	setText(FolderColumn, img.imageFileFolder());

	QString format;
	switch (img.params()._fmt)
	{
	case JPG:
		format = "JPG";
		break;
	case BMP:
		format = "BMP";
		break;
	case PNG:
		format = "PNG";
		break;
	case GIF:
		format = "GIF";
		break;
	case TIFF:
		format = "TIFF";
		break;
	case UNKN:
		format = "Unknown";
		break;
	default:
		format = "Unknown";
		break;
	}

	setText(ImageFormatColumn, format);
}

bool QtImageListItem::operator<( const QTreeWidgetItem &other ) const
{
	if (treeWidget()->sortColumn() == DimensionsColumn || treeWidget()->sortColumn() == FileSizeColumn)
		return data(treeWidget()->sortColumn(), Qt::UserRole).toUInt() < other.data(treeWidget()->sortColumn(), Qt::UserRole).toUInt();
	else
		return QTreeWidgetItem::operator<(other);
}

void QtImageListItem::setCurrent( bool current /*= true*/ )
{
	setText(MarkerColumn, current ? QString::fromUtf16((const ushort*)currentWpMarkSymbol) : QString());
}
