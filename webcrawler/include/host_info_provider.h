#pragma once

#include "requester.h"

namespace WebCrawler
{

class HostInfoProvider : public QObject
{
	Q_OBJECT

public:
	HostInfoProvider();

	Q_INVOKABLE void handleRequest(Common::RequesterSharedPtr requester);
	Q_INVOKABLE void stopRequestHandling(Common::RequesterSharedPtr requester);
};

}