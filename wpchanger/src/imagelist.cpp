#include "imagelist.h"

#include <QDebug>
#include <QFile>

#include <algorithm>
#include <fstream>
#include <string>

ImageList::ImageList()
{
}

size_t ImageList::size() const
{
	return _list.size();
}

void ImageList::addImage(const Image &image)
{
	_list.push_back(image);
	INVOKE_CALLBACK(listChanged, size() - 1);
}

void ImageList::removeImages(const std::vector<size_t> &indexes)
{
	decltype(_list) newList;

	for (size_t i = 0; i < _list.size(); ++i)
	{
		if (std::find(indexes.begin(), indexes.end(), i) == indexes.end())
			newList.push_back(_list[i]);
	}
	_list = newList;

	INVOKE_CALLBACK(listChanged, invalid_index);
}

void ImageList::clear()
{
	INVOKE_CALLBACK(listCleared);
	_list.clear();
}

bool ImageList::empty() const
{
	return _list.empty();
}

Image &ImageList::operator [](size_t index)
{
	return _list[index];
}

const Image &ImageList::operator [](size_t index) const
{
	return _list[index];
}

bool ImageList::saveList( const QString& filename ) const
{
	std::ofstream file(filename.toStdWString().c_str(), std::ios_base::binary);
	if (!file.is_open())
		return false;

	for (size_t i = 0; i < _list.size(); ++i)
	{
		const QByteArray path = _list[i].imageFilePath().toUtf8();
		const int pathSize = path.size();
		file.write((const char*)&pathSize, sizeof (pathSize));
		file.write(path.constData(), path.size());
		file.write((const char*)&_list[i].params(), sizeof(ImgParams));
	}

	return true;
}

bool ImageList::loadList( const QString& filename )
{
	std::ifstream file(filename.toStdWString().c_str(), std::ios_base::binary);
	if (!file.is_open())
		return false;

	clear();

	char path[3000] = {0};
	int pathLength = 0;
	file.read((char*)&pathLength, sizeof (pathLength));
	if (pathLength >= sizeof(path) / sizeof(char))
		return false; // Path is too long

	file.read(path, pathLength);
	while (!file.eof())
	{
		const QString imageFileName = QString::fromUtf8(path, pathLength);
		ImgParams params;
		file.read((char*)&params, sizeof(params));
		_list.push_back(Image(imageFileName, params));
		file.read((char*)&pathLength, sizeof (pathLength));
		file.read(path, pathLength);
	}

	INVOKE_CALLBACK(listChanged, invalid_index);
	return true;
}

//Deletes corresponding files from disk and removes from the list if deletion successful
bool ImageList::deleteFilesFromDisk(const std::vector<size_t> &indexes)
{
	decltype(_list) newList;

	for (size_t i = 0; i < _list.size(); ++i)
	{
		if (std::find(indexes.begin(), indexes.end(), i) == indexes.end())
			newList.push_back(_list[i]);
		else 
		{
			QFile file (_list[i].imageFilePath());
			file.setPermissions(file.permissions() | QFile::WriteUser);
			if (!file.remove())
			{
				qDebug() << "failed to remove: " << file.errorString();
				newList.push_back(_list[i]);
			}
			else
			{
				qDebug() << "Deleted: " << file.fileName();
			}
		}
	}

	_list = newList;

	INVOKE_CALLBACK(listChanged, invalid_index);

	return true;
}
