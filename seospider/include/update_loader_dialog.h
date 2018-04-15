#pragma once

#include "ui_updates_loader_dialog_content.h"
#include "requester_wrapper.h"
#include "url.h"

namespace CrawlerEngine
{

struct DownloadResponse;
struct DownloadProgressResponse;
class Requester;

}

namespace SeoSpider
{

class UpdateLoaderDialog : public QFrame
{
private:
	enum class UnitType
	{
		UnitTypeBytes,
		UnitTypeKB,
		UnitTypeMB
	};

public:
	UpdateLoaderDialog(const QString& downloadLink, QWidget* parent = nullptr);

protected:
	virtual void showEvent(QShowEvent* event) override;

private slots:
	void onDownloadNowClicked();
	void onDownloadLaterClicked();
	void onAboutDownloadProgress(CrawlerEngine::Requester* requester, const CrawlerEngine::DownloadProgressResponse& response);
	void onUpdatesDownloadingFinished(CrawlerEngine::Requester* requester, const CrawlerEngine::DownloadResponse& response);

private:
	void closeDialog() noexcept;
	std::pair<double, QString> calculateDownloadSpeed(quint64 bytesReceived) const;
	std::tuple<double, double, QString> downloadStatus(quint64 bytesReceived, quint64 bytesTotal) const;

	UnitType unit(quint64 bytesCount) const noexcept;
	QString unitToString(UnitType unitType) const;

	double downloadPercents(const CrawlerEngine::DownloadProgressResponse& response) const;
	QString downloadStatusString(const CrawlerEngine::DownloadProgressResponse& response) const;
	QString downloadSpeedString(const CrawlerEngine::DownloadProgressResponse& response) const;
	double fromUnitToUnit(double value, UnitType from, UnitType to) const;

private:
	Ui_UpdatesLoaderDialogContent* m_ui;
	CrawlerEngine::Url m_downloadLink;
	CrawlerEngine::RequesterWrapper m_downloadRequester;
	QTime m_downloadTime;
};

}