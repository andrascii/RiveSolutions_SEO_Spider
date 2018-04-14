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

class UpdatesLoaderDialog : public QFrame
{
public:
	UpdatesLoaderDialog(const QString& downloadLink, QWidget* parent = nullptr);

protected:
	virtual void showEvent(QShowEvent* event) override;

private slots:
	void onDownloadNowClicked();
	void onDownloadLaterClicked();
	void onAboutDownloadProgress(CrawlerEngine::Requester* requester, const CrawlerEngine::DownloadProgressResponse& response);
	void onUpdatesDownloadingFinished(CrawlerEngine::Requester* requester, const CrawlerEngine::DownloadResponse& response);

private:
	void closeDialog() noexcept;
	QString updatesPatchSaveDirPath() const;

private:
	Ui_UpdatesLoaderDialogContent* m_ui;
	CrawlerEngine::Url m_downloadLink;
	CrawlerEngine::RequesterWrapper m_downloadRequester;
	QTime m_downloadTime;
};

}