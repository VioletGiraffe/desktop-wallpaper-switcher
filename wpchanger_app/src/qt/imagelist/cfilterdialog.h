#pragma once

#include <QDialog>

namespace Ui {
	class CFilterDialog;
}

class CFilterDialog : public QDialog
{
	Q_OBJECT

public:
	explicit CFilterDialog(QWidget *parent = 0);
	~CFilterDialog();

	void display();

signals:
	void filterTextChanged(QString text);

protected:
	void closeEvent(QCloseEvent* e) override;

private:
	Ui::CFilterDialog *ui;
};

