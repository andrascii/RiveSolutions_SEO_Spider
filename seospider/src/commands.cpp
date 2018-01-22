#include "commands.h"
#include "xlsxdocument.h"
#include "xlsxformat.h"
#include "xlsxrichstring.h"
#include "sequenced_data_collection.h"
#include "application.h"
#include "storage_adapter_factory.h"
#include "parsed_page_info.h"

namespace SeoSpider
{

bool ICommand::isCompound() const noexcept
{
	return false;
}

/*=============================================================================================*/

CompoundCommand::CompoundCommand(const char* description, const QIcon& icon)
	: m_description(description)
	, m_icon(icon)
{
}

const std::vector<SeoSpider::ICommandPointer>& CompoundCommand::commands() const noexcept
{
	return m_commands;
}

void CompoundCommand::addCommand(ICommandPointer commandPointer)
{
	m_commands.push_back(commandPointer);
}

QIcon CompoundCommand::icon() const
{
	return m_icon;
}

const char* CompoundCommand::description() const noexcept
{
	return m_description;
}

void CompoundCommand::execute()
{
	for (ICommandPointer command : m_commands)
	{
		command->execute();
	}
}

bool CompoundCommand::isCompound() const noexcept
{
	return true;
}

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

	QXlsx::Document xlsxDocument(path);

	int rowNumber = 1;

	QXlsx::Format headerFormat;
	headerFormat.setFontColor(Qt::red);
	headerFormat.setFontSize(13);

	QXlsx::Format columnFormat;
	columnFormat.setFontColor(Qt::red);
	columnFormat.setFontSize(13);

	for (int i = 0, sz = (int)m_storageDescriptions.size(); i < sz; ++i)
	{
		const CrawlerEngine::StorageType storageType = m_storageDescriptions[i].storageType;
		const ISequencedStorage& sequencedStorage = *m_dataCollection->storage(storageType);
		const int storageSize = sequencedStorage.size();
		
		QXlsx::RichString headerRichString;
		headerRichString.addFragment(m_storageDescriptions[i].storageTypeDescriptionName + QStringLiteral(" (%1 items)").arg(storageSize), headerFormat);

		xlsxDocument.write(rowNumber++, 1, headerRichString);

		QVector<ParsedPageInfo::Column> columnsForType =
			StorageAdapterFactory::parsedPageAvailableColumns(static_cast<StorageAdapterType>(storageType));

		for (int k = 1; k <= columnsForType.size(); ++k)
		{
			QXlsx::RichString columnRichString;
			columnRichString.addFragment(ParsedPageInfo::itemTypeDescription(columnsForType[k - 1]), columnFormat);

			xlsxDocument.write(rowNumber, k, columnRichString);
		}

		++rowNumber;

		for (int j = 0; j < storageSize; ++j, ++rowNumber)
		{
			ParsedPageInfo parsedPageInfoProvider(sequencedStorage[j]);

			for (int k = 1; k <= columnsForType.size(); ++k)
			{
				xlsxDocument.write(rowNumber, k, parsedPageInfoProvider.itemValue(columnsForType[k - 1]));
			}
		}

		++rowNumber;
	}

	xlsxDocument.save();
}

}