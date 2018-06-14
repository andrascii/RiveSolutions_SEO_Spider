#include "feedback_dialog.h"
#include "smtp_sender.h"

namespace SeoSpider
{

FeedbackDialog::FeedbackDialog(QWidget* parent)
	: Dialog(parent)
	, m_ui(new Ui::FeedbackDialog)
{
	m_ui->setupUi(this);

	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
	flags = flags & (~helpFlag);
	setWindowFlags(flags);

	VERIFY(connect(m_ui->sendButton, &QPushButton::clicked, this, &FeedbackDialog::onSendFeedbackClicked));
	VERIFY(connect(m_ui->textEdit, &QTextEdit::textChanged, this, &FeedbackDialog::onTextEditChanged));

	m_ui->headerLabel->setObjectName("feedbackDialogHeaderLabel");

	onTextEditChanged();
}

void FeedbackDialog::onSendFeedbackClicked()
{
	Common::SmtpSettings settings;
	settings.setEmailUsername("rivesolutionreports");
	settings.setEmailPassword("Riv3S#lutions");
	settings.setEmailSender("rivesolutionreports@yandex.ru");
	settings.setEmailSmtpPort(465);
	settings.setEmailSmtpHost("smtp.yandex.ru");
	settings.setEmailRecipients("feedback@rivesolutions.com");
	settings.setEmailUseSsl(true);
	settings.setEmailUseAuthentication(true);

	QString info = m_ui->textEdit->toPlainText();
	QTextCodec* codec = QTextCodec::codecForName("latin1");
	info = codec->makeDecoder()->toUnicode(info.toUtf8());

	Common::SmtpMessage message(settings, "Feedback", info);
	Common::SmtpSender::send(message, QString(), this, SLOT(onSendingFinished(const QString&, int, const QByteArray&)));
	close();
}

void FeedbackDialog::onTextEditChanged()
{
	m_ui->sendButton->setDisabled(m_ui->textEdit->toPlainText().isEmpty());
}

}