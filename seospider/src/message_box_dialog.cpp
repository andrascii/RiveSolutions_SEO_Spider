#include "message_box_dialog.h"
#include "helpers.h"
#include "application.h"
#include "main_window.h"
#include "widget_helpers.h"
#include "shadow_decoration_frame.h"
#include "cursor_factory.h"
#include "dialog_container.h"

namespace SeoSpider
{

MessageBoxDialog::MessageBoxDialog(QWidget* parent)
	: Dialog(parent)
	, m_ui(new Ui_MessageBox)
	, m_clickedButtonRole(QDialogButtonBox::InvalidRole)
{
	m_ui->setupUi(this);

	VERIFY(connect(m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject())));
	VERIFY(connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept())));
	VERIFY(connect(m_ui->buttonBox, &QDialogButtonBox::clicked, this, &MessageBoxDialog::onButtonClicked));

	m_ui->buttonBox->setCursor(CursorFactory::createCursor(Qt::PointingHandCursor));
	m_ui->messageLabel->setWordWrap(true);
}

void MessageBoxDialog::setMessage(const QString& message)
{
	m_ui->messageLabel->setText(message);

	adjustSize();

	setMinimumSize(size());
	setMaximumSize(size());
}

void MessageBoxDialog::setStandardButtons(QDialogButtonBox::StandardButtons buttons)
{
	m_ui->buttonBox->setStandardButtons(buttons);
}

void MessageBoxDialog::onButtonClicked(QAbstractButton* button)
{
	m_clickedButtonRole = m_ui->buttonBox->buttonRole(button);
}

void MessageBoxDialog::showEvent(QShowEvent* event)
{
	WidgetHelpers::moveWidgetToHostCenter(this, theApp->mainWindow());

	resize(size());

	QFrame::showEvent(event);
}

// bool MessageBoxDialog::eventFilter(QObject*, QEvent* event)
// {
// 	if (event->type() == QEvent::MouseButtonPress)
// 	{
// 		//FloatingFrame::mousePressEvent(static_cast<QMouseEvent*>(event));
// 	}
// 	
// 	if (event->type() == QEvent::MouseButtonRelease)
// 	{
// 		//FloatingFrame::mouseReleaseEvent(static_cast<QMouseEvent*>(event));
// 	}
// 	
// 	if (event->type() == QEvent::MouseMove)
// 	{
// 		//FloatingFrame::mouseMoveEvent(static_cast<QMouseEvent*>(event));
// 	}
// 
// 	return false;
// }

}