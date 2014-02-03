#include "csettings.h"

std::shared_ptr<QSettings> CSettings::_settings;
QMutex CSettings::_settingsMutex;

CSettings::CSettings()
{
	QMutexLocker locker (&_settingsMutex);
	if (_settings.get() == 0)
	{
		QApplication::setOrganizationName("VGSoft");
		QApplication::setApplicationName("WPChanger");
		_settings = std::make_shared<QSettings>(QSettings::NativeFormat, QSettings::UserScope, "VGSoft", "WPChanger");
	}
}

void CSettings::setValue(const QString &key, const QVariant &value)
{
	QMutexLocker locker (&_settingsMutex);
	_settings->setValue( key, value );
}

QVariant CSettings::value(const QString &key, const QVariant &defaultValue) const
{
	QMutexLocker locker (&_settingsMutex);
	return _settings->value( key, defaultValue );
}
