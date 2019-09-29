#pragma once
#ifdef ENABLE_SCREENSHOTS
#include "iscreenshot_maker.h"

namespace CrawlerEngineTests
{

class TestsScreenshotMaker : public QObject, public IScreenshotMaker
{
	Q_OBJECT

public:
	TestsScreenshotMaker(QObject* parent = nullptr);

	Q_INVOKABLE virtual void handleRequest(RequesterSharedPtr requester) override;
	Q_INVOKABLE virtual void stopRequestHandling(RequesterSharedPtr requester) override;
	virtual QObject* qobject() override;
};

}
#endif