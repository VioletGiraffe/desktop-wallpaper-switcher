#ifndef IMAGETHUMBNAILWIDGET_H
#define IMAGETHUMBNAILWIDGET_H

#include <QWidget>
#include <QString>
#include <QImage>

#include "image.h"

class ImageThumbnailWidget : public QWidget
{
	Q_OBJECT
public:
	explicit ImageThumbnailWidget (QWidget *parent = 0);

	// Will store image resized for this widget's size at the moment of assigning image, instead of storing full image and resizing when drawing to screen
	void setThumbMode ();

	bool displayImage (const Image& image);

protected:
	void paintEvent  (QPaintEvent* e);

private:
	//The image for which to draw a thumbnail
	QImage _imgDrawer;
	bool   _resize;
};

#endif // IMAGETHUMBNAILWIDGET_H
