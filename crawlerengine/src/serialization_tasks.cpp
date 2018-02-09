#include "serialization_tasks.h"

namespace CrawlerEngine
{
SerializationTask::SerializationTask(std::unique_ptr<Serializer> serializer, const QString& fileName)
	: m_result(std::make_unique<SerializationTaskResponseResult>(std::move(serializer)))
	, m_fileName(fileName)
{
}

void SerializationTask::run()
{
	try
	{
		QFileInfo fileInfo(m_fileName);

		const QString fileDirPath = fileInfo.absolutePath();

		const QString baseFilename = fileInfo.completeBaseName();

		const QString fileSuffix = fileInfo.suffix();

		const QString additionalString = QDateTime::currentDateTime().toString(QString("yyyyMMdd_hhmmsszzz"));

		const QString tempFileName = QString("%1/%2_tmp_%3.%4")
			.arg(fileDirPath)
			.arg(baseFilename)
			.arg(additionalString)
			.arg(fileSuffix);

		const QString oldFileName = QString("%1/%2_old_%3.%4")
			.arg(fileDirPath)
			.arg(baseFilename)
			.arg(additionalString)
			.arg(fileSuffix);

		QFile file(tempFileName);

		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			throw std::runtime_error(file.errorString().toStdString());
		}

		m_result->serializer->saveToStream(file);

		file.close();

		QFile oldFile(m_fileName);

		const bool oldFileExisits = oldFile.exists();

		if (oldFileExisits)
		{
			if (!oldFile.rename(oldFileName))
			{
				throw std::runtime_error(oldFile.errorString().toStdString());
			}
		}
		QFile::rename(tempFileName, m_fileName);

		if (oldFileExisits)
		{
			oldFile.remove();
		}
	}
	catch (const std::exception& e)
	{
		ERRLOG << e.what();
		m_result->error = QString(e.what());
	}
}

std::unique_ptr<TaskResponseResult> SerializationTask::result()
{
	return std::move(m_result);
}

DeserializatoinTask::DeserializatoinTask(std::unique_ptr<Serializer> serializer, const QString& fileName)
	: m_result(std::make_unique<SerializationTaskResponseResult>(std::move(serializer)))
	, m_fileName(fileName)
{
}

void DeserializatoinTask::run()
{
	try
	{
		QFile file(m_fileName);

		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			throw std::runtime_error(file.errorString().toStdString());
		}

		m_result->serializer->loadFromStream(file);

	}
	catch (const std::exception& e)
	{
		ERRLOG << e.what();
		m_result->error = QString(e.what());
	}

}

std::unique_ptr<TaskResponseResult> DeserializatoinTask::result()
{
	return std::move(m_result);
}

}
