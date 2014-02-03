#include "image.h"
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QHash>

Image::Image() :_id(0), _isValid(false)
{
}

Image::Image( const QString& filename, ImgParams params /* = ImgParams() */) :
	_params(params)
{
	loadFromFile(filename);
}

bool Image::isValidImage() const
{
	return _isValid;
}

bool Image::loadFromFile( const QString& filename )
{
	_filePath = filename;
	QFileInfo info (filename);
	_name = info.fileName();
	_folder = info.dir().path();

	if (_params != ImgParams())
	{
		_isValid = true;
	}
	else
	{
		QImageReader reader (filename);
		if (_isValid = reader.canRead())
		{
			_params._width = reader.size().width();
			_params._height = reader.size().height();

			//Determine the image format by file extension
			const QString extension = info.suffix().toLower();
			if (extension == "jpg" || extension == "jpeg")
				_params._fmt = JPG;
			else if (extension == "bmp")
				_params._fmt = BMP;
			else if (extension == "gif")
				_params._fmt = GIF;
			else if (extension == "jpg")
				_params._fmt = JPG;
			else if (extension == "tiff")
				_params._fmt = TIFF;
			else if (extension == "xbm")
				_params._fmt = XBM;
			else if (extension == "xpm")
				_params._fmt = XPM;
			else if (extension == "png")
				_params._fmt = PNG;

			_params._fileSize = info.size();
		}
	}

	_id = (size_t)qHash(_filePath) ^ (size_t)this;

	return _isValid;
}

const QString& Image::imageFilePath() const
{
	return _filePath;
}

const QString& Image::imageFileFolder() const
{
	return _folder;
}

const QString& Image::imageFileName() const
{
	return _name;
}

QImage Image::constructQImageObject() const
{
	QImage qImg;
	if (_isValid)
	{
		qImg.load(_filePath);
	}

	return qImg;
}

ImgParams Image::params() const
{
	return _params;
}

WPOPTIONS Image::stretchMode () const
{
	return _params._wpDisplayMode;
}

void Image::setStretchMode( WPOPTIONS mode ) const
{
	_params._wpDisplayMode = mode;
}

size_t Image::id() const
{
	return _id;
}
