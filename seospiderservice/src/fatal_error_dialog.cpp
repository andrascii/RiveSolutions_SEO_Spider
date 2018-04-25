#include "fatal_error_dialog.h"

namespace SeoSpiderService
{

FatalErrorDialog::FatalErrorDialog(QWidget* parent)
	: QDialog(parent)
	, m_ui(new Ui::FatalErrorDialog)
	, m_sendReportsNow(false)
{
	m_ui->setupUi(this);
	
	VERIFY(connect(m_ui->sendReportNowPushButton, &QPushButton::clicked, this, [this]() { m_sendReportsNow = true; }));
	VERIFY(connect(m_ui->sendReportNowPushButton, &QPushButton::clicked, this, &QDialog::accept));
	VERIFY(connect(m_ui->sendReportLaterPushButton, &QPushButton::clicked, this, &QDialog::reject));
}

bool FatalErrorDialog::sendReportsNow() const
{
	return m_sendReportsNow;
}

QString FatalErrorDialog::detailedInformation() const
{
	return m_ui->detailedInfoTextEdit->toPlainText();
}

}