#ifndef CSETTINGS_H
#define CSETTINGS_H

#include "../QtIncludes"
#include <memory>

class QSettings;

class CSettings
{
public:
	CSettings();

	void setValue(const QString &key, const QVariant &value);
	QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;

private:
	static std::shared_ptr<QSettings> _settings;
	static QMutex _settingsMutex;
};

#endif // CSETTINGS_H
