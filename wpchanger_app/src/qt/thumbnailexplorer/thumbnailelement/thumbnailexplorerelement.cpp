#include "thumbnailexplorerelement.h"
#include "ui_thumbnailexplorerelement.h"

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
