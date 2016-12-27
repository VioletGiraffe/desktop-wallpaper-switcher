#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QDialog>
RESTORE_COMPILER_WARNINGS

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
