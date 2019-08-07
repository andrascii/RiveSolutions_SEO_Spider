#pragma once

#include "itask_processor.h"

namespace CrawlerEngine
{
	
class TaskProcessor: public QObject, public ITaskProcessor
{
	Q_OBJECT
public:
	TaskProcessor();

	Q_INVOKABLE virtual void handleRequest(RequesterSharedPtr requester) override;

	Q_INVOKABLE virtual void stopRequestHandling(RequesterSharedPtr requester) override;

	virtual QObject* qobject() override;

};

}
