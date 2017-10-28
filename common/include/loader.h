#pragma once

#include "irequest.h"
#include "iresponse.h"
#include "requester.h"

namespace Common
{

class Loader : public QObject
{
	Q_OBJECT

public:
	Loader(QObject* parent = nullptr);
	Loader(const Loader& other);

public slots:
	void handleRequest(RequesterSharedPtr requester);
	void stopRequestHandling(RequesterSharedPtr requester);
	void onLoadingDone(QNetworkReply* reply);

private:
	QNetworkAccessManager* m_networkAccessor;
	RequesterSharedPtr m_requester;
};

}