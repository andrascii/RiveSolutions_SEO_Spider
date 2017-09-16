#include "fatal_error_dialog.h"

namespace WatchDogService
{

FatalErrorDialog::FatalErrorDialog(QWidget* parent)
	: QDialog(parent)
	, m_ui(new Ui::FatalErrorDialog)
{
	m_ui->setupUi(this);

	VERIFY(connect(m_ui->sendReportNowPushButton, &QPushButton::clicked, this, &QDialog::accept));
	VERIFY(connect(m_ui->sendReportLaterPushButton, &QPushButton::clicked, this, &QDialog::reject));
}

}