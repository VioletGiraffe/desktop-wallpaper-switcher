#pragma once

#include "image.h"

DISABLE_COMPILER_WARNINGS
#include <QWidget>
RESTORE_COMPILER_WARNINGS

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
