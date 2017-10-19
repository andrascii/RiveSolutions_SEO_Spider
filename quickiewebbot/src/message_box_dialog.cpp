#include "message_box_dialog.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

MessageBoxDialog::MessageBoxDialog(QWidget* parent)
	: QFrame(parent)
	, m_ui(new Ui_MessageBox)
{
	m_ui->setupUi(this);

	setWindowFlags(Qt::Dialog);
	setWindowModality(Qt::WindowModal);

	const QSize preferredSize(QuickieWebBotHelpers::pointsToPixels(270), QuickieWebBotHelpers::pointsToPixels(80));

	setMinimumSize(preferredSize);
	setMaximumSize(preferredSize);

	VERIFY(connect(m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject())));
	VERIFY(connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept())));
}

void MessageBoxDialog::setMessage(const QString& message) const
{
	m_ui->messageLabel->setText(message);
}

void MessageBoxDialog::setIcon(Icon icon) const
{
	const QSize pixmapSize(QuickieWebBotHelpers::pointsToPixels(20), QuickieWebBotHelpers::pointsToPixels(20));

	switch (icon)
	{
		case InformationIcon:
		{
			static QPixmap s_informationIcon(":/images/information-message-icon.png");
			m_ui->imageLabel->setPixmap(s_informationIcon.scaled(pixmapSize));
			break;
		}
		case WarningIcon:
		{
			static QPixmap s_warningIcon(":/images/warning-message-icon.png");
			m_ui->imageLabel->setPixmap(s_warningIcon.scaled(pixmapSize));
			break;
		}
		case CriticalErrorIcon:
		{
			static QPixmap s_criticalErrorIcon(":/images/critical-error-message-icon.png");
			m_ui->imageLabel->setPixmap(s_criticalErrorIcon.scaled(pixmapSize));
			break;
		}
		default:
		{
			DEBUG_ASSERT(!"Invalid icon");
		}
	}
}

int MessageBoxDialog::result() const
{
	return m_dialogCode;
}

void MessageBoxDialog::accept()
{
	done(QDialog::Accepted);
}

void MessageBoxDialog::reject()
{
	done(QDialog::Rejected);
}

void MessageBoxDialog::done(int r)
{
	m_dialogCode = static_cast<QDialog::DialogCode>(r);

	hide();
}

void MessageBoxDialog::showEvent(QShowEvent* event)
{
	QuickieWebBotHelpers::moveWidgetToHostCenter(this);
	QFrame::showEvent(event);
}

}