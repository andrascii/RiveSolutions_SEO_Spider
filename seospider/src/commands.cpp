#include "commands.h"
#include "xlsxdocument.h"
#include "xlsxformat.h"
#include "xlsxrichstring.h"
#include "sequenced_data_collection.h"
#include "application.h"
#include "storage_adapter_factory.h"
#include "main_window.h"
#include "isequenced_storage.h"
#include "robots_txt_loader.h"
#include "helpers.h"
#include "crawler.h"

namespace SeoSpider
{

bool ICommand::canExecute() const noexcept
{
	return true;
}

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
	return "Export Data to .xlsx File";
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


ExportUrlInfoToXlsxCommand::ExportUrlInfoToXlsxCommand(const CrawlerEngine::ISequencedStorage* storage,
	const QVector<ParsedPageInfo::Column>& columns,
	int row)
	: m_storage(storage)
	, m_availableColumns(columns)
	, m_row(row)
{
	DEBUG_ASSERT(m_row >= 0);
}

QIcon ExportUrlInfoToXlsxCommand::icon() const
{
	return QIcon();
}

const char* ExportUrlInfoToXlsxCommand::description() const noexcept
{
	return "Export Url Info to .xlsx";
}

void ExportUrlInfoToXlsxCommand::execute()
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

	ParsedPageInfo parsedPageInfoProvider(m_storage->get(m_row));

	int rowNumber = 1;

	for (ParsedPageInfo::Column column : m_availableColumns)
	{
		xlsxDocument.write(rowNumber, 1, ParsedPageInfo::itemTypeDescription(column));
		xlsxDocument.write(rowNumber, 2, parsedPageInfoProvider.itemValue(column));

		++rowNumber;
	}

	xlsxDocument.save();
}



ExportUrlOutlinksToXlsxCommand::ExportUrlOutlinksToXlsxCommand(const CrawlerEngine::ISequencedStorage* storage, int row)
	: m_storage(storage)
	, m_row(row)
{
}

QIcon ExportUrlOutlinksToXlsxCommand::icon() const
{
	return QIcon();
}

const char* ExportUrlOutlinksToXlsxCommand::description() const noexcept
{
	return "Export Url Outlinks to .xlsx";
}

void ExportUrlOutlinksToXlsxCommand::execute()
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

	ParsedPageInfo parsedPageInfoProvider(m_storage->get(m_row));

	int rowNumber = 1;

	xlsxDocument.write(rowNumber++, 1, QString("Outlinks for ") + parsedPageInfoProvider.itemValue(ParsedPageInfo::Column::UrlColumn).toString());

	for (std::size_t i = 0; i < parsedPageInfoProvider.linksCount(PageLinkContext::LinksOnThisPage); ++i)
	{
		xlsxDocument.write(rowNumber++, 1, parsedPageInfoProvider.itemValue(ParsedPageInfo::PageLinksColumn::UrlColumn, PageLinkContext::LinksOnThisPage, i));
	}

	xlsxDocument.save();
}



ExportUrlInlinksToXlsxCommand::ExportUrlInlinksToXlsxCommand(const CrawlerEngine::ISequencedStorage* storage, int row)
	: m_storage(storage)
	, m_row(row)
{
}

QIcon ExportUrlInlinksToXlsxCommand::icon() const
{
	return QIcon();
}

const char* ExportUrlInlinksToXlsxCommand::description() const noexcept
{
	return "Export Url Inlinks to .xlsx";
}

void ExportUrlInlinksToXlsxCommand::execute()
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

	ParsedPageInfo parsedPageInfoProvider(m_storage->get(m_row));

	int rowNumber = 1;

	xlsxDocument.write(rowNumber++, 1, QString("Inlinks for ") + parsedPageInfoProvider.itemValue(ParsedPageInfo::Column::UrlColumn).toString());

	for (std::size_t i = 0; i < parsedPageInfoProvider.linksCount(PageLinkContext::LinksToThisPage); ++i)
	{
		xlsxDocument.write(rowNumber++, 1, parsedPageInfoProvider.itemValue(ParsedPageInfo::PageLinksColumn::UrlColumn, PageLinkContext::LinksToThisPage, i));
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


QIcon OpenRobotsTxtFileCommand::icon() const
{
	return QIcon();
}

const char* OpenRobotsTxtFileCommand::description() const noexcept
{
	return "Open robots.txt File";
}

void OpenRobotsTxtFileCommand::execute()
{
	const CrawlerEngine::RobotsTxtLoader* loader = 
		Common::Helpers::fast_cast<const CrawlerEngine::RobotsTxtLoader*>(theApp->crawler()->robotsTxtLoader());

	const CrawlerEngine::Url url = loader->robotsTxtUrl();

	if (loader->isValid() && url.isValid())
	{
		QDesktopServices::openUrl(url);
	}
}


RefreshPageCommand::RefreshPageCommand(CrawlerEngine::StorageType storageType, int index)
	: m_storageType(storageType)
	, m_index(index)
{
}

QIcon RefreshPageCommand::icon() const
{
	return QIcon();
}

const char* RefreshPageCommand::description() const noexcept
{
	return "Refresh Page";
}

void RefreshPageCommand::execute()
{
	theApp->crawler()->refreshPage(m_storageType, m_index);
}

bool RefreshPageCommand::canExecute() const noexcept
{
	return theApp->crawler()->canRefreshPage();
}

}