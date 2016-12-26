#include "imagethumbnailwidget.h"
#include "image.h"

#include <QPainter>
#include <QImage>
#include <QDebug>

#include <time.h>

ImageThumbnailWidget::ImageThumbnailWidget(QWidget *parent) :
	_resize(false),
	QWidget(parent)
{
}

// Will store image resized for this widget's size at the moment of assigning image, instead of storing full image and resizing when drawing to screen
void ImageThumbnailWidget::setThumbMode()
{
	_resize = true;
}

bool ImageThumbnailWidget::displayImage(const Image& image)
{
	if (!image.isValidImage())
		return false;

	_imgDrawer = _resize ? image.constructQImageObject().scaled(width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation) : image.constructQImageObject();
	update();
	return true;
}

void ImageThumbnailWidget::paintEvent(QPaintEvent* /*e*/)
{
	if (_imgDrawer.isNull()) return;
	QPainter(this).drawImage(0, 0, _resize ? _imgDrawer : _imgDrawer.scaled(width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

	//	qDebug() << __FUNCTION__ << " " << stop-start << " ms";
}
