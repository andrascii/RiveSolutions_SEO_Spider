#include "message_box_dialog.h"
#include "helpers.h"
#include "application.h"
#include "main_window.h"

namespace SeoSpider
{

MessageBoxDialog::MessageBoxDialog()
	: QFrame(nullptr)
	, m_ui(new Ui_MessageBox)
	, m_clickedButtonRole(QDialogButtonBox::InvalidRole)
{
	m_ui->setupUi(this);

	setWindowFlags(Qt::Dialog);
	setWindowModality(Qt::ApplicationModal);
	setAttribute(Qt::WA_DeleteOnClose, true);

	VERIFY(connect(m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject())));
	VERIFY(connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept())));
	VERIFY(connect(m_ui->buttonBox, &QDialogButtonBox::clicked, this, &MessageBoxDialog::onButtonClicked));
}

void MessageBoxDialog::setMessage(const QString& message)
{
	m_ui->messageLabel->setText(message);

	adjustSize();

	setMinimumSize(size());
	setMaximumSize(size());
}

void MessageBoxDialog::setIcon(Icon icon)
{
	const QSize pixmapSize(Common::Helpers::pointsToPixels(20), Common::Helpers::pointsToPixels(20));

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

void MessageBoxDialog::setStandardButtons(QDialogButtonBox::StandardButtons buttons)
{
	m_ui->buttonBox->setStandardButtons(buttons);
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

void MessageBoxDialog::onButtonClicked(QAbstractButton* button)
{
	m_clickedButtonRole = m_ui->buttonBox->buttonRole(button);
}

void MessageBoxDialog::showEvent(QShowEvent* event)
{
	Common::Helpers::moveWidgetToHostCenter(this);

	theApp->mainWindow()->setDisabled(true);

	QFrame::showEvent(event);
}

void MessageBoxDialog::hideEvent(QHideEvent* event)
{
	theApp->mainWindow()->setDisabled(false);

	QFrame::hideEvent(event);

	emit dialogClosed(m_clickedButtonRole);
}

}