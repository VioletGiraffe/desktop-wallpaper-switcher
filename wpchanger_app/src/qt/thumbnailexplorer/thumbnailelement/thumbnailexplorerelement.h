#pragma once

#include "image.h"

#include <QWidget>

namespace Ui {
	class ThumbnailExplorerElement;
}

class ThumbnailExplorerElement : public QWidget
{
public:
	explicit ThumbnailExplorerElement(const Image& img = Image(), QWidget *parent = 0);
	~ThumbnailExplorerElement();

private:
	Ui::ThumbnailExplorerElement *ui;
};
