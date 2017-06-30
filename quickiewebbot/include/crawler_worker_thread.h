#pragma once

#include "model_controller.h"

namespace QuickieWebBot
{

class CrawlerWorkerThread : public QObject
{
	Q_OBJECT

public:
	CrawlerWorkerThread(QObject* parent = nullptr);
	
	void setupModelController(ModelController* pModelController);

	Q_INVOKABLE void start();

private:

private:
	std::atomic_bool m_onAboutToStop;
	ModelController* m_modelController;

	QNetworkAccessManager m_networkAccessManager;
	QNetworkRequest m_networkRequest;
};

}
