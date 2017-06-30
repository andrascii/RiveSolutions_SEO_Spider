#pragma once

#include "imodel_controller.h"
#include "qurl_hasher.h"

namespace QuickieWebBot
{

class ModelController 
	: public QObject
	, public IModelController
{
	Q_OBJECT

public:
	ModelController(QObject* parent = nullptr);

	virtual void setUrl(QUrl const& url) override;
	virtual QUrl const& url() const override;



public slots:
	//void addNewUrl(QString url);

private:

	struct CrawlerQueues
	{
		std::unordered_set<std::shared_ptr<QUrl>, Common::QUrlHasher> internalUrlsQueue;
		std::unordered_set<std::shared_ptr<QUrl>, Common::QUrlHasher> crawledUrlsQueue;
		std::unordered_set<std::shared_ptr<QUrl>, Common::QUrlHasher> externalUrlsQueue;
	};

	struct GuiQueues
	{
		std::vector<std::shared_ptr<QUrl>> internalUrlsQueue;
		std::vector<std::shared_ptr<QUrl>> crawledUrlsQueue;
		std::vector<std::shared_ptr<QUrl>> externalUrlsQueue;
	};

	CrawlerQueues m_crawlerQueues;
	GuiQueues m_guiQueues;
	QUrl m_url;
};

}