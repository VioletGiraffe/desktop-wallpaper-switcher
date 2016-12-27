#ifndef IMAGETHUMBNAILWIDGET_H
#define IMAGETHUMBNAILWIDGET_H

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QImage>
#include <QWidget>
RESTORE_COMPILER_WARNINGS

class Image;

class ImageThumbnailWidget : public QWidget
{
public:
	explicit ImageThumbnailWidget(QWidget *parent = 0);

	// Will store image resized for this widget's size at the moment of assigning image, instead of storing full image and resizing when drawing to screen
	void setThumbMode();

	bool displayImage (const Image& image);

protected:
	void paintEvent  (QPaintEvent* e) override;

private:
	//The image for which to draw a thumbnail
	QImage _imgDrawer;
	bool   _resize;
};

#endif // IMAGETHUMBNAILWIDGET_H
