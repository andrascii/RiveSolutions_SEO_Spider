#include "update_loader_dialog.h"
#include "application.h"
#include "helpers.h"
#include "main_window.h"
#include "download_request.h"
#include "download_response.h"
#include "update_helpers.h"

namespace SeoSpider
{

UpdateLoaderDialog::UpdateLoaderDialog(const QString& downloadLink, QWidget* parent)
	: QFrame(parent)
	, m_ui(new Ui_UpdatesLoaderDialogContent)
	, m_downloadLink(downloadLink)
{
	m_ui->setupUi(this);

	m_ui->progressBar->hide();
	m_ui->progressBar->setTextVisible(true);

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
	if (!m_downloadLink.isValid())
	{
		ERRLOG << "Incorrect download link. Check the " << UpdateHelpers::downloadAddressFileUrl() << " server file.";

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

	m_downloadRequester.reset(request, this, &UpdateLoaderDialog::onAboutDownloadProgress, &UpdateLoaderDialog::onUpdatesDownloadingFinished);
	m_downloadRequester->start();
	m_downloadTime.start();

	m_ui->downloadNowButton->setDisabled(true);
	m_ui->downloadLaterButton->setText("Cancel Downloading");
}

void UpdateLoaderDialog::onDownloadLaterClicked()
{
	closeDialog();
}

void UpdateLoaderDialog::onAboutDownloadProgress(CrawlerEngine::Requester*, const CrawlerEngine::DownloadProgressResponse& response)
{
	m_ui->progressBar->setValue(static_cast<int>(downloadPercents(response)));
	m_ui->progressBar->setFormat(downloadStatusString(response) + " (" + downloadSpeedString(response) + ")");
}

void UpdateLoaderDialog::onUpdatesDownloadingFinished(CrawlerEngine::Requester*, const CrawlerEngine::DownloadResponse& response)
{
	m_ui->progressBar->setValue(100);

	const QString filepath = UpdateHelpers::updatesPatchSaveDirPath() + "/" + m_downloadLink.fileName();

	QFile file(filepath);
	file.open(QIODevice::WriteOnly);

	DEBUG_ASSERT(response.hopsChain.length() > 0);

	if (response.hopsChain.length() == 0)
	{
		ERRLOG << "For some reason hops chain is empty";
		return;
	}

	const CrawlerEngine::Hop lastHop = response.hopsChain.back();

	file.write(lastHop.body());
	file.close();

	closeDialog();

	emit updateDownloaded(filepath);
}

void UpdateLoaderDialog::closeDialog() noexcept
{
	if (m_downloadRequester.get())
	{
		m_downloadRequester->stop();
	}

	m_downloadRequester.reset();

	theApp->mainWindow()->setDisabled(false);

	close();
}

std::pair<double, QString> UpdateLoaderDialog::calculateDownloadSpeed(quint64 bytesReceived) const
{
	UnitType unitType = unit(bytesReceived);
	const QString unitString = unitToString(unitType) + "/s";
	const double speed = fromUnitToUnit(bytesReceived * 1000.0 / m_downloadTime.elapsed(), UnitType::UnitTypeBytes, unitType);

	return std::make_pair(speed, unitString);
}

std::tuple<double, double, QString> UpdateLoaderDialog::downloadStatus(quint64 bytesReceived, quint64 bytesTotal) const
{
	const UnitType unitType = unit(bytesTotal);
	const QString unitString = unitToString(unitType);
	const double downloaded = fromUnitToUnit(bytesReceived, UnitType::UnitTypeBytes, unitType);
	const double total = fromUnitToUnit(bytesTotal, UnitType::UnitTypeBytes, unitType);

	return std::make_tuple(downloaded, total, unitString);
}

UpdateLoaderDialog::UnitType UpdateLoaderDialog::unit(quint64 bytesCount) const noexcept
{
	if (bytesCount < 1024)
	{
		return UnitType::UnitTypeBytes;
	}
	else if (bytesCount < 1024 * 1024)
	{
		return UnitType::UnitTypeKB;
	}
	
	return UnitType::UnitTypeMB;
}

QString UpdateLoaderDialog::unitToString(UnitType unitType) const
{
	switch (unitType)
	{
		case UnitType::UnitTypeBytes:
		{
			return QString("bytes");
		}
		case UnitType::UnitTypeKB:
		{
			return QString("kB");
		}
		case UnitType::UnitTypeMB:
		{
			return QString("MB");
		}
	}

	ASSERT(!"Unknown unit type");
	return QString();
}

double UpdateLoaderDialog::downloadPercents(const CrawlerEngine::DownloadProgressResponse& response) const
{
	return static_cast<double>(response.bytesReceived) / static_cast<double>(response.bytesTotal + 1) * 100.0;
}

QString UpdateLoaderDialog::downloadStatusString(const CrawlerEngine::DownloadProgressResponse& response) const
{
	std::tuple<double, double, QString> status = downloadStatus(response.bytesReceived, response.bytesTotal);

	return QString::number(std::get<0>(status), 'f', 2) + std::get<2>(status) + "/" + QString::number(std::get<1>(status), 'f', 2) + std::get<2>(status);
}

QString UpdateLoaderDialog::downloadSpeedString(const CrawlerEngine::DownloadProgressResponse& response) const
{
	std::pair<double, QString> downloadSpeed = calculateDownloadSpeed(response.bytesTotal);

	return QString::number(downloadSpeed.first, 'f', 2) + " " + downloadSpeed.second;
}

double UpdateLoaderDialog::fromUnitToUnit(double value, UnitType from, UnitType to) const
{
	double result = 0;

	switch (from)
	{
		case UnitType::UnitTypeBytes:
		{
			result = value;
			break;
		}
		case UnitType::UnitTypeKB:
		{
			result = value * 1024;
			break;
		}
		case UnitType::UnitTypeMB:
		{
			result = value * 1024 * 1024;
			break;
		}
		default:
		{
			ASSERT(!"Unknown unit type");
		}
	}

	switch (to)
	{
		case UnitType::UnitTypeBytes:
		{
			break;
		}
		case UnitType::UnitTypeKB:
		{
			result /= 1024;
			break;
		}
		case UnitType::UnitTypeMB:
		{
			result /= 1024 * 1024;
			break;
		}
		default:
		{
			ASSERT(!"Unknown unit type");
		}
	}

	return result;
}

}