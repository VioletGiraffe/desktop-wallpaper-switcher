#pragma once

#include <QDialog>

namespace Ui {
	class SettingsDialog;
}

class SettingsDialog : public QDialog
{
public:
	explicit SettingsDialog(QWidget *parent = 0);
	~SettingsDialog();

public:
	void accept() override;

private:
	Ui::SettingsDialog *ui;
};
