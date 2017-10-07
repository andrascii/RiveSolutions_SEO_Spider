#include "message_box_dialog.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

MessageBoxDialog::MessageBoxDialog(QWidget* parent)
	: QDialog(parent)
	, m_ui(new Ui_MessageBoxDialog)
{
	m_ui->setupUi(this);

	setWindowModality(Qt::WindowModal);
}

void MessageBoxDialog::setMessage(const QString& message)
{
	m_ui->messageLabel->setText(message);
}

void MessageBoxDialog::setIcon(Icon icon)
{
	const QSize pixmapSize(QuickieWebBotHelpers::pointsToPixels(16), QuickieWebBotHelpers::pointsToPixels(16));

	switch (icon)
	{
		case InformationIcon:
		{
			static QPixmap s_informationIcon(pixmapSize);

			if (!s_informationIcon)
			{
				s_informationIcon.load(":/images/information-message-icon.png");
			}

			m_ui->imageLabel->setPixmap(s_informationIcon);
			break;
		}
		case WarningIcon:
		{
			static QPixmap s_warningIcon(pixmapSize);

			if (!s_warningIcon)
			{
				s_warningIcon.load(":/images/warning-message-icon.png");
			}

			m_ui->imageLabel->setPixmap(s_warningIcon);
			break;
		}
		case CriticalErrorIcon:
		{
			static QPixmap s_criticalErrorIcon(pixmapSize);

			if (!s_criticalErrorIcon)
			{
				s_criticalErrorIcon.load(":/images/critical-error-message-icon.png");
			}

			m_ui->imageLabel->setPixmap(s_criticalErrorIcon);
			break;
		}
		default:
		{
			DEBUG_ASSERT(!"Invalid icon");
		}
	}
}

}