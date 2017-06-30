#pragma once

#include "imodel_controller.h"

namespace QuickieWebBot
{

class CrawlerWorkerThread : public QObject
{
	Q_OBJECT

public:
	CrawlerWorkerThread(QObject* parent = nullptr);
	
	Q_INVOKABLE void start(IModelController* pModelController);

private:


private:
	std::atomic_bool m_onAboutToStop;

	QNetworkAccessManager m_networkAccessManager;
	QNetworkRequest m_networkRequest;
};

}
