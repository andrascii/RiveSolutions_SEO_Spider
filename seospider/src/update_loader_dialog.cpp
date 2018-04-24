#include "update_loader_dialog.h"
#include "application.h"
#include "helpers.h"
#include "main_window.h"
#include "update_helpers.h"

namespace SeoSpider
{

UpdateLoaderDialog::UpdateLoaderDialog(QWidget* parent)
	: QFrame(parent)
	, m_ui(new Ui_UpdatesLoaderDialogContent)
{
	m_ui->setupUi(this);

	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	setWindowModality(Qt::ApplicationModal);
	setAttribute(Qt::WA_DeleteOnClose, true);

	VERIFY(connect(m_ui->downloadNowButton, &QPushButton::clicked, this, &UpdateLoaderDialog::onDownloadNowClicked));
	VERIFY(connect(m_ui->downloadLaterButton, &QPushButton::clicked, this, &UpdateLoaderDialog::onDownloadLaterClicked));
}

void UpdateLoaderDialog::showEvent(QShowEvent* event)
{
	Common::Helpers::moveWidgetToHostCenter(this);

	theApp->mainWindow()->setDisabled(true);

	setEnabled(true);

	QFrame::showEvent(event);
}

void UpdateLoaderDialog::onDownloadNowClicked()
{
	//if (!m_downloadLink.isValid())
	//{
	//	ERRLOG << "Incorrect download link. Check the " << UpdateHelpers::downloadAddressFileUrl() << " server file.";
	//
	//	closeDialog();
	//
	//	return;
	//}
	//
	//m_ui->progressBar->show();
	//m_ui->progressBar->setValue(0);
	//
	//m_updateLoader->startDownloadingUpdate(CrawlerEngine::Url(m_downloadLink));
	//
	//m_ui->downloadNowButton->setDisabled(true);
	//m_ui->downloadLaterButton->setText("Cancel Downloading");

	if (QProcess::startDetached("Uninstall SeoSpider.exe", { "--updater" }))
	{
		theApp->quit();
	}
}

void UpdateLoaderDialog::onDownloadLaterClicked()
{
	closeDialog();
}

void UpdateLoaderDialog::closeDialog() noexcept
{
	theApp->mainWindow()->setDisabled(false);

	close();
}

}