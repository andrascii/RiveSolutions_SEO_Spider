#include "stdafx.h"
#include "task_processor.h"
#include "handler_registry.h"
#include "task_request.h"
#include "thread_message_dispatcher.h"
#include "task_response.h"

namespace CrawlerEngine
{

TaskProcessor::TaskProcessor()
{
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, RequestType::RequestTypeRunTask);
}

void TaskProcessor::handleRequest(RequesterSharedPtr requester)
{
	ASSERT(requester->request()->requestType() == RequestType::RequestTypeRunTask);
	TaskRequest* request = static_cast<TaskRequest*>(requester->request());

	request->task->run();

	IResponseSharedPtr response = std::make_shared<TaskResponse>(request->task->result());
	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, response);
}

void TaskProcessor::stopRequestHandling(RequesterSharedPtr requester)
{
	Q_UNUSED(requester);
}

QObject* TaskProcessor::qobject()
{
	return this;
}

}
