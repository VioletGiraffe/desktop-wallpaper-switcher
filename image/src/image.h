#ifndef IMAGE_H
#define IMAGE_H

#include <QString>
#include <QImage>

class QWidget;

enum IMGFORMAT {JPG, BMP, PNG, GIF, TIFF, XBM, XPM, UNKN};
enum WPOPTIONS {CENTERED, STRETCHED, SYSTEM_DEFAULT};
struct ImgParams
{
	ImgParams () : _width (0), _height(0), _fileSize(0), _fmt(UNKN), _wpDisplayMode(STRETCHED) {}
	bool operator== (const ImgParams& other) const { return _width == other._width && _height == other._height && _fileSize == other._fileSize && _fmt == other._fmt && _wpDisplayMode == other._wpDisplayMode; }
	bool operator!= (const ImgParams& other) const { return !operator==(other); }
	int _width, _height;
	int _fileSize;
	IMGFORMAT _fmt;
	WPOPTIONS _wpDisplayMode;
} ;

//Class for Image object
class Image
{
public:
	Image();

	//Creates Image object for the specified image file
	Image(const QString& filename, ImgParams params = ImgParams());

	bool isValidImage () const;

	//Associates this object with another image file. Returns whether the file is a valid image or not
	bool loadFromFile(const QString& filename);

	//
	const QString& imageFilePath () const;
	const QString& imageFileFolder () const;
	const QString& imageFileName () const;
	QImage constructQImageObject () const;
	ImgParams params() const;

	//
	WPOPTIONS stretchMode () const;
	void setStretchMode (WPOPTIONS mode) const;

	qulonglong id() const;

private:
	//The name of file this Image object is associated with
	QString _filePath;
	QString _name;
	QString _folder;

	qulonglong _id;

	//Is this Image object associated with a valid file?
	bool _isValid;

	//Properties of the image
	mutable ImgParams _params;

};

#endif // IMAGE_H
