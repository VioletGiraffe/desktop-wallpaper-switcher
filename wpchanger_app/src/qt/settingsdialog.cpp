#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "settings.h"
#include "settings/csettings.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	ui->_spSwitchInterval->setValue(CSettings().value(SETTINGS_INTERVAL, SETTINGS_DEFAULT_INTERVAL).toInt());
	ui->_cbRandomize->setChecked(CSettings().value(SETTINGS_RANDOMIZE, SETTINGS_DEFAULT_RANDOMIZE).toBool());
	ui->_cbAutostartSwitching->setChecked(CSettings().value(SETTINGS_START_SWITCHING_ON_STARTUP, SETTINGS_DEFAULT_AUTOSTART).toBool());

	connect(this, SIGNAL(accepted()), SLOT(accepted()));
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::accepted()
{
	CSettings s;
	s.setValue(SETTINGS_INTERVAL, ui->_spSwitchInterval->value());
	s.setValue(SETTINGS_RANDOMIZE, ui->_cbRandomize->isChecked());
	s.setValue(SETTINGS_START_SWITCHING_ON_STARTUP, ui->_cbAutostartSwitching->isChecked());
}
