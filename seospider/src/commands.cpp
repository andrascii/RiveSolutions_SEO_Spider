#include "commands.h"
#include "xlsxdocument.h"
#include "xlsxformat.h"
#include "xlsxrichstring.h"
#include "sequenced_data_collection.h"
#include "application.h"
#include "storage_adapter_factory.h"
#include "parsed_page_info.h"
#include "main_window.h"

namespace SeoSpider
{

bool ICommand::isCompound() const noexcept
{
	return false;
}



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



CopyToClipboardAllPagesCommand::CopyToClipboardAllPagesCommand(const CrawlerEngine::ISequencedStorage* storage)
	: m_storage(storage)
{
}

QIcon CopyToClipboardAllPagesCommand::icon() const
{
	return QIcon();
}

const char* CopyToClipboardAllPagesCommand::description() const noexcept
{
	return "Copy to Clipboard All Pages";
}

void CopyToClipboardAllPagesCommand::execute()
{
	QClipboard* clipboard = QApplication::clipboard();

	QString clipboardText;

	for (int i = 0, sz = m_storage->size(); i < sz; ++i)
	{
		ParsedPageInfo parsedPageInfoProvider((*m_storage)[i]);

		clipboardText += parsedPageInfoProvider.itemValue(ParsedPageInfo::Column::UrlColumn).toString() + "\n";
	}

	clipboard->setText(clipboardText);
}



CopyToClipboardAllColumnsDataCommand::CopyToClipboardAllColumnsDataCommand(
	const CrawlerEngine::ISequencedStorage* storage, 
	CrawlerEngine::StorageType storageType, 
	int pageIndex)
	: m_storage(storage)
	, m_storageType(storageType)
	, m_pageIndex(pageIndex)
{
}

QIcon CopyToClipboardAllColumnsDataCommand::icon() const
{
	return QIcon();
}

const char* CopyToClipboardAllColumnsDataCommand::description() const noexcept
{
	return "Copy to Clipboard All Columns Data";
}

void CopyToClipboardAllColumnsDataCommand::execute()
{
	ASSERT(m_pageIndex < m_storage->size());

	QClipboard* clipboard = QApplication::clipboard();
	QString clipboardText;
	ParsedPageInfo parsedPageInfoProvider((*m_storage)[m_pageIndex]);

	QVector<ParsedPageInfo::Column> columnsForType =
		StorageAdapterFactory::parsedPageAvailableColumns(static_cast<StorageAdapterType>(m_storageType));

	for (int i = 0, sz = columnsForType.size(); i < sz; ++i)
	{
		clipboardText += parsedPageInfoProvider.itemValue(columnsForType[i]).toString() + " ";
	}

	clipboard->setText(clipboardText);
}



CopyToClipboardUrlCommand::CopyToClipboardUrlCommand(const CrawlerEngine::ISequencedStorage* storage, int pageIndex)
	: m_storage(storage)
	, m_pageIndex(pageIndex)
{
}

QIcon CopyToClipboardUrlCommand::icon() const
{
	return QIcon();
}

const char* CopyToClipboardUrlCommand::description() const noexcept
{
	return "Copy to Clipboard Url";
}

void CopyToClipboardUrlCommand::execute()
{
	ASSERT(m_pageIndex < m_storage->size());

	QClipboard* clipboard = QApplication::clipboard();

	clipboard->setText((*m_storage)[m_pageIndex]->url.toDisplayString());
}



CheckGoogleCacheCommand::CheckGoogleCacheCommand(const QUrl& url)
	: m_url(url)
{
}

QIcon CheckGoogleCacheCommand::icon() const
{
	return QIcon();
}

const char* CheckGoogleCacheCommand::description() const noexcept
{
	return "Check Google Cache";
}

void CheckGoogleCacheCommand::execute()
{
	//
	// TODO: move this link to .cfg file
	//
	QDesktopServices::openUrl(QStringLiteral("http://webcache.googleusercontent.com/search?q=cache:%1").arg(m_url.toDisplayString()));
}



CheckHTMLWithW3CValidatorCommand::CheckHTMLWithW3CValidatorCommand(const QUrl& url)
	: m_url(url)
{
}

QIcon CheckHTMLWithW3CValidatorCommand::icon() const
{
	return QIcon();
}

const char* CheckHTMLWithW3CValidatorCommand::description() const noexcept
{
	return "Check HTML with W3C Validator";
}

void CheckHTMLWithW3CValidatorCommand::execute()
{
	//
	// TODO: move this link to .cfg file
	//
	QDesktopServices::openUrl(QStringLiteral("https://validator.w3.org/check?uri=%1").arg(m_url.toDisplayString()));
}



OpenInWaybackMachineCommand::OpenInWaybackMachineCommand(const QUrl& url)
	: m_url(url)
{
}

QIcon OpenInWaybackMachineCommand::icon() const
{
	return QIcon();
}

const char* OpenInWaybackMachineCommand::description() const noexcept
{
	return "Open in Wayback Machine";
}

void OpenInWaybackMachineCommand::execute()
{
	//
	// TODO: move this link to .cfg file
	//
	QDesktopServices::openUrl(QStringLiteral("http://web.archive.org/web/*/%1").arg(m_url.toDisplayString()));
}



ShowOtherDomainsOnIpCommand::ShowOtherDomainsOnIpCommand(const QByteArray& ipAddress)
	: m_ipAddress(ipAddress)
{
}

QIcon ShowOtherDomainsOnIpCommand::icon() const
{
	return QIcon();
}

const char* ShowOtherDomainsOnIpCommand::description() const noexcept
{
	return "Show Other Domains on IP";
}

void ShowOtherDomainsOnIpCommand::execute()
{
	//
	// TODO: move this link to .cfg file
	//
	QDesktopServices::openUrl(QStringLiteral("https://www.bing.com/search?q=ip:%1").arg(QString(m_ipAddress)));
}

}