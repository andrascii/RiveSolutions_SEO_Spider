#pragma once

#include "irequest.h"

namespace CrawlerEngine
{

struct TaskResponseResult;

class ITask
{
public:
	virtual ~ITask() = default;
	virtual void run() = 0;
	virtual std::unique_ptr<TaskResponseResult> result() = 0;
};

struct TaskRequest : public IRequest
{
	TaskRequest(std::shared_ptr<ITask> task)
		: task(task)
	{
	}

	virtual IRequest* clone() const override
	{
		return new TaskRequest(*this);
	}
	virtual RequestType requestType() const noexcept override
	{
		return RequestType::RequestTypeRunTask;
	}

	std::shared_ptr<ITask> task;
};

}