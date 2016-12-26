#include "cfilterdialog.h"
#include "ui_cfilterdialog.h"

#include <QShortcut>
#include <QTimer>

#include <assert.h>

CFilterDialog::CFilterDialog(QWidget *parent) :
	QDialog(parent, Qt::Popup),
	ui(new Ui::CFilterDialog)
{
	ui->setupUi(this);

	connect(ui->_lineEdit, &QLineEdit::textEdited, this, &CFilterDialog::filterTextChanged);

	new QShortcut(QKeySequence("Esc"), this, SLOT(close()), SLOT(close()), Qt::WidgetWithChildrenShortcut);
}

CFilterDialog::~CFilterDialog()
{
	delete ui;
}

void CFilterDialog::display()
{
	setGeometry(QRect(QPoint(1, 1), size()));
	show();
	QTimer::singleShot(0, ui->_lineEdit, SLOT(setFocus()));
	QTimer::singleShot(0, ui->_lineEdit, SLOT(selectAll()));
	emit filterTextChanged(ui->_lineEdit->text());
}

void CFilterDialog::closeEvent(QCloseEvent * e)
{
	emit filterTextChanged(QString());
	QDialog::closeEvent(e);
}
