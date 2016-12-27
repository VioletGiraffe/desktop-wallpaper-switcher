#include "thumbnailexplorerelement.h"

DISABLE_COMPILER_WARNINGS
#include "ui_thumbnailexplorerelement.h"
RESTORE_COMPILER_WARNINGS

ThumbnailExplorerElement::ThumbnailExplorerElement(const Image& img /*= Image()*/, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ThumbnailExplorerElement)
{
	ui->setupUi(this);

	ui->_imageView->setThumbMode();
	ui->_imageName->setText(img.imageFileName());
	ui->_imageView->displayImage(img);
}

ThumbnailExplorerElement::~ThumbnailExplorerElement()
{
	delete ui;
}
