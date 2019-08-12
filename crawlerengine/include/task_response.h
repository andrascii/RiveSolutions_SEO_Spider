#pragma once
#include "stdafx.h"

#include "iresponse.h"

namespace CrawlerEngine
{

struct TaskResponseResult
{
	virtual ~TaskResponseResult() = default;
};

struct TaskResponse : public IResponse
{
	TaskResponse(std::shared_ptr<TaskResponseResult> result)
		: result(result)
	{
	}
	
	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseTypeTaskResult;
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseTypeTaskResult)

	std::shared_ptr<TaskResponseResult> result;
};

}