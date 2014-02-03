#ifndef THUMBNAILEXPLORERELEMENT_H
#define THUMBNAILEXPLORERELEMENT_H

#include <QWidget>
#include "image.h"

namespace Ui {
	class ThumbnailExplorerElement;
}

class ThumbnailExplorerElement : public QWidget
{
	Q_OBJECT

public:
	explicit ThumbnailExplorerElement(const Image& img = Image(), QWidget *parent = 0);
	~ThumbnailExplorerElement();

private:
	Ui::ThumbnailExplorerElement *ui;
};

#endif // THUMBNAILEXPLORERELEMENT_H
