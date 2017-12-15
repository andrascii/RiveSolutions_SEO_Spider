#pragma once
#include "task_response.h"
#include "serializer.h"
#include "task_request.h"

namespace CrawlerEngine
{
	
struct SerializationTaskResponseResult : public TaskResponseResult
{
	SerializationTaskResponseResult(std::shared_ptr<Serializer> serializer)
		: serializer(serializer)
	{
		
	}
	std::shared_ptr<Serializer> serializer;
};


class SerializationTask: public ITask
{
public:
	SerializationTask(std::shared_ptr<Serializer> serializer, const QString& fileName);
	virtual void run() override;
	virtual std::shared_ptr<TaskResponseResult> result() override;

private:
	std::shared_ptr<SerializationTaskResponseResult> m_result;
	QString m_fileName;
};

}
