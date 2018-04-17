#include "update_loader_dialog.h"
#include "application.h"
#include "helpers.h"
#include "main_window.h"
#include "update_helpers.h"
#include "update_loader.h"

namespace SeoSpider
{

UpdateLoaderDialog::UpdateLoaderDialog(const QString& downloadLink, QWidget* parent)
	: QFrame(parent)
	, m_ui(new Ui_UpdatesLoaderDialogContent)
	, m_downloadLink(downloadLink)
	, m_updateLoader(new UpdateLoader(this))
{
	m_ui->setupUi(this);

	m_ui->progressBar->hide();
	m_ui->progressBar->setTextVisible(true);

	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	setWindowModality(Qt::ApplicationModal);
	setAttribute(Qt::WA_DeleteOnClose, true);

	VERIFY(connect(m_ui->downloadNowButton, &QPushButton::clicked, this, &UpdateLoaderDialog::onDownloadNowClicked));
	VERIFY(connect(m_ui->downloadLaterButton, &QPushButton::clicked, this, &UpdateLoaderDialog::onDownloadLaterClicked));

	VERIFY(connect(m_updateLoader, &UpdateLoader::downloadProgress, this, &UpdateLoaderDialog::onAboutDownloadProgress));
	VERIFY(connect(m_updateLoader, &UpdateLoader::updateDownloaded, this, &UpdateLoaderDialog::onUpdatesDownloadingFinished));
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

	m_updateLoader->startDownloadingUpdate(CrawlerEngine::Url(m_downloadLink));

	m_ui->downloadNowButton->setDisabled(true);
	m_ui->downloadLaterButton->setText("Cancel Downloading");
}

void UpdateLoaderDialog::onDownloadLaterClicked()
{
	closeDialog();
}

void UpdateLoaderDialog::onAboutDownloadProgress(quint64 bytesReceived, quint64 bytesTotal, double downloadSpeed)
{
	m_ui->progressBar->setValue(static_cast<int>(downloadPercents(bytesReceived, bytesTotal)));
	m_ui->progressBar->setFormat(downloadStatusString(bytesReceived, bytesTotal) + " (" + downloadSpeedString(downloadSpeed) + ")");
}

void UpdateLoaderDialog::onUpdatesDownloadingFinished(const QString& filepath)
{
	m_ui->progressBar->setValue(100);

	emit updateDownloaded(filepath);
}

void UpdateLoaderDialog::closeDialog() noexcept
{
	m_updateLoader->stopDownloading();

	theApp->mainWindow()->setDisabled(false);

	close();
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

double UpdateLoaderDialog::downloadPercents(quint64 bytesReceived, quint64 bytesTotal) const
{
	return static_cast<double>(bytesReceived) / static_cast<double>(bytesTotal + 1) * 100.0;
}

QString UpdateLoaderDialog::downloadStatusString(quint64 bytesReceived, quint64 bytesTotal) const
{
	std::tuple<double, double, QString> status = downloadStatus(bytesReceived, bytesTotal);

	return QString::number(std::get<0>(status), 'f', 2) + std::get<2>(status) + "/" + QString::number(std::get<1>(status), 'f', 2) + std::get<2>(status);
}

QString UpdateLoaderDialog::downloadSpeedString(double downloadSpeed) const
{
	UnitType unitType = unit(downloadSpeed);
	const QString unitString = unitToString(unitType) + "/s";

	return QString::number(fromUnitToUnit(downloadSpeed, UnitType::UnitTypeBytes, unitType), 'f', 2) + " " + unitString;
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