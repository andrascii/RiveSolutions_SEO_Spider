#include "widget_under_mouse_info_dialog.h"

namespace QuickieWebBot
{

WidgetUnderMouseInfoDialog::WidgetUnderMouseInfoDialog(QWidget* parent)
	: QDialog(parent)
	, m_ui(new Ui_WidgetUnderMouseInfoDialog)
{
	m_ui->setupUi(this);

	setWindowModality(Qt::ApplicationModal);
}

void WidgetUnderMouseInfoDialog::closeEvent(QCloseEvent* event)
{
	done(DialogCode::Accepted);

	QDialog::closeEvent(event);
}

void WidgetUnderMouseInfoDialog::showWithInfo(const QString& objectName, const QString& className)
{
	m_ui->objectNameLabel->setText(objectName);
	m_ui->classNameLabel->setText(className);

	adjustSize();

	show();
}

}