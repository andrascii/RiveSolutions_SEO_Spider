#include "updates_loader_dialog.h"
#include "application.h"
#include "helpers.h"
#include "main_window.h"
#include "download_request.h"
#include "download_response.h"

namespace SeoSpider
{

UpdatesLoaderDialog::UpdatesLoaderDialog(const QString& downloadLink, QWidget* parent)
	: QFrame(parent)
	, m_ui(new Ui_UpdatesLoaderDialogContent)
	, m_downloadLink(downloadLink)
{
	downloadLink;
	m_ui->setupUi(this);

	m_ui->progressBar->hide();
	m_ui->progressBar->setTextVisible(true);

	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	setWindowModality(Qt::ApplicationModal);
	setAttribute(Qt::WA_DeleteOnClose, true);

	VERIFY(connect(m_ui->downloadNowButton, &QPushButton::clicked, this, &UpdatesLoaderDialog::onDownloadNowClicked));
	VERIFY(connect(m_ui->downloadLaterButton, &QPushButton::clicked, this, &UpdatesLoaderDialog::onDownloadLaterClicked));
}

void UpdatesLoaderDialog::showEvent(QShowEvent* event)
{
	Common::Helpers::moveWidgetToHostCenter(this);

	theApp->mainWindow()->setDisabled(true);

	setEnabled(true);

	QFrame::showEvent(event);
}

void UpdatesLoaderDialog::onDownloadNowClicked()
{
	if (!m_downloadLink.isValid())
	{
		ERRLOG << "Incorrect download link. Check the http://rivesolutions.com/download_address.txt server file.";

		closeDialog();

		return;
	}

	m_ui->progressBar->show();
	m_ui->progressBar->setValue(0);

	CrawlerEngine::CrawlerRequest crawlerRequest
	{
		m_downloadLink,
		CrawlerEngine::DownloadRequestType::RequestTypeGet
	};

	CrawlerEngine::DownloadRequest request(
		crawlerRequest, 
		CrawlerEngine::DownloadRequest::LinkStatus::LinkStatusFirstLoading, 
		CrawlerEngine::DownloadRequest::BodyProcessingCommand::CommandDownloadBodyAnyway
	);

	m_downloadRequester.reset(request, this, &UpdatesLoaderDialog::onAboutDownloadProgress, &UpdatesLoaderDialog::onUpdatesDownloadingFinished);
	m_downloadRequester->start();
	m_downloadTime.start();

	m_ui->downloadLaterButton->setText("Cancel Downloading");
}

void UpdatesLoaderDialog::onDownloadLaterClicked()
{
	closeDialog();
}

void UpdatesLoaderDialog::onAboutDownloadProgress(CrawlerEngine::Requester*, const CrawlerEngine::DownloadProgressResponse& response)
{
	double speed = response.bytesReceived * 1000.0 / m_downloadTime.elapsed();
	double downloaded = response.bytesReceived;
	double total = response.bytesTotal;

	QString unit;

	if (speed < 1024)
	{
		unit = "bytes/sec";
	}
	else if (speed < 1024 * 1024)
	{
		speed /= 1024;
		unit = "kB/s";
	}
	else
	{
		speed /= 1024 * 1024;
		unit = "MB/s";
	}

	QString sizeUnit;

	if (response.bytesTotal < 1024)
	{
		sizeUnit = "bytes";
	}
	else if (response.bytesTotal < 1024 * 1024)
	{
		downloaded /= 1024;
		total /= 1024;
		sizeUnit = "kB";
	}
	else
	{
		downloaded /= 1024 * 1024;
		total /= 1024 * 1024;
		sizeUnit = "MB";
	}

	const double percents = static_cast<double>(response.bytesReceived) / static_cast<double>(response.bytesTotal + 1) * 100.0;
	m_ui->progressBar->setValue(static_cast<int>(percents));
	m_ui->progressBar->setFormat(QString::number(downloaded, 'f', 3) + sizeUnit + "/" + QString::number(total, 'f', 3) + sizeUnit + " (" + QString::number(speed) + " " + unit + ")");
}

void UpdatesLoaderDialog::onUpdatesDownloadingFinished(CrawlerEngine::Requester*, const CrawlerEngine::DownloadResponse& response)
{
	m_ui->progressBar->setValue(100);

	QFile file(updatesPatchSaveDirPath() + "/" + m_downloadLink.fileName());
	file.open(QIODevice::WriteOnly);

	DEBUG_ASSERT(response.hopsChain.length() > 0);

	if (response.hopsChain.length() == 0)
	{
		ERRLOG << "For some reason hops chain is empty";
		return;
	}

	const CrawlerEngine::Hop lastHop = response.hopsChain.back();

	QDataStream stream(&file);
	stream.writeBytes(lastHop.body().data(), lastHop.body().size());
}

void UpdatesLoaderDialog::closeDialog() noexcept
{
	if (m_downloadRequester.get())
	{
		m_downloadRequester->stop();
	}

	m_downloadRequester.reset();

	theApp->mainWindow()->setDisabled(false);

	close();
}

QString UpdatesLoaderDialog::updatesPatchSaveDirPath() const
{
	QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	path = QDir::cleanPath(path + QString("/RiveSolutions/SeoSpider/Update/"));

	QDir dir(path);

	if (!dir.exists())
	{
		dir.mkpath(path);
	}

	return path;
}

}