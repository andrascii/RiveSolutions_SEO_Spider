#include "commands.h"
#include "xlsxdocument.h"
#include "sequenced_data_collection.h"
#include "application.h"

namespace SeoSpider
{

/*=============================================================================================*/

OpenUrlCommand::OpenUrlCommand(const QUrl& url)
	: m_url(url)
{
}

QIcon OpenUrlCommand::icon() const
{
	return QIcon();
}

const char* OpenUrlCommand::description() const noexcept
{
	return "Open URL";
}

void OpenUrlCommand::execute()
{
	QDesktopServices::openUrl(m_url);
}

ICommand::CommandType OpenUrlCommand::type() const noexcept
{
	return OpenUrlCommandType;
}

/*=============================================================================================*/

RemoveRowCommand::RemoveRowCommand(int row)
	: m_row(row)
{
	ASSERT(m_row > 0);
}

QIcon RemoveRowCommand::icon() const
{
	return QIcon();
}

const char* RemoveRowCommand::description() const noexcept
{
	return "Remove Row";
}

void RemoveRowCommand::execute()
{

}

ICommand::CommandType RemoveRowCommand::type() const noexcept
{
	return RemoveRowCommandType;
}

/*=============================================================================================*/

ExportDataToXlsxCommand::ExportDataToXlsxCommand(const CrawlerEngine::SequencedDataCollection* dataCollection,
	const std::vector<DCStorageDescription>& storageDescriptions)
	: m_dataCollection(dataCollection)
	, m_storageDescriptions(storageDescriptions)
{
}

QIcon ExportDataToXlsxCommand::icon() const
{
	return QIcon();
}

const char* ExportDataToXlsxCommand::description() const noexcept
{
	return "Export Data To .xlsx File";
}

void ExportDataToXlsxCommand::execute()
{
	const QString path = QFileDialog::getSaveFileName(
		theApp->mainWindow(), 
		QObject::tr("Save To Excel"), 
		QString(), 
		QString("*.xlsx")
	);

	if (path.isEmpty())
	{
		return;
	}

	QXlsx::Document xlsx(path);

	std::size_t rowNumber = 1;

	for (std::size_t i = 0, sz = m_storageDescriptions.size(); i < sz; ++i)
	{
		const CrawlerEngine::StorageType storageType = m_storageDescriptions[i].storageType;
		const std::size_t storageSize = m_dataCollection->storage(storageType)->size();

		xlsx.write(QStringLiteral("A%1").arg(rowNumber++), 
			m_storageDescriptions[i].storageTypeDescriptionName + QStringLiteral("(%1 items)").arg(storageSize));

		for (std::size_t j = 0; j < storageSize; ++j)
		{
			xlsx.write(QStringLiteral("A%1").arg(rowNumber++), (*m_dataCollection->storage(storageType))[(int)j]->url);
		}
	}

	xlsx.save();
}

ICommand::CommandType ExportDataToXlsxCommand::type() const noexcept
{
	return ExportDataToXlsxCommandType;
}

}