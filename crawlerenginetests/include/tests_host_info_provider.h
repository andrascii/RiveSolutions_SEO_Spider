#pragma once

#include "ihost_info_provider.h"

namespace CrawlerEngineTests
{

class TestsHostInfoProvider : public QObject, public IHostInfoProvider
{
	Q_OBJECT

public:
	TestsHostInfoProvider();
	~TestsHostInfoProvider();

	Q_INVOKABLE virtual void handleRequest(RequesterSharedPtr requester) override;
	Q_INVOKABLE virtual void stopRequestHandling(RequesterSharedPtr requester) override;
	virtual QObject* qobject() override;
};

}