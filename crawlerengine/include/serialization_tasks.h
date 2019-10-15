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
	QString error;
};


class SerializationTask: public ITask
{
public:
	SerializationTask(std::unique_ptr<Serializer> serializer, const QString& fileName);
	virtual void run() override;
	virtual std::unique_ptr<TaskResponseResult> result() override;
	virtual const QString& fileName() const override;

private:
	std::unique_ptr<SerializationTaskResponseResult> m_result;
	QString m_fileName;
};


class DeserializatoinTask: public ITask
{
public:
	DeserializatoinTask(std::unique_ptr<Serializer> serializer, const QString& fileName);
	virtual void run() override;
	virtual std::unique_ptr<TaskResponseResult> result() override;
	virtual const QString& fileName() const override;

private:
	std::unique_ptr<SerializationTaskResponseResult> m_result;
	QString m_fileName;
};

}
