#pragma once

#include "data_collection_groups_factory.h"
#include "parsed_page_info.h"

namespace CrawlerEngine
{

class SequencedDataCollection;
class ISequencedStorage;

}

namespace SeoSpider
{

class ICommand
{
public:
	virtual ~ICommand() = default;

	virtual QIcon icon() const = 0;
	virtual const char* description() const noexcept = 0;
	virtual void execute() = 0;
	virtual bool canExecute() const noexcept;
	virtual bool isCompound() const noexcept;
};

using ICommandPointer = std::shared_ptr<ICommand>;

class CompoundCommand : public ICommand
{
public:
	CompoundCommand(const char* description, const QIcon& icon = QIcon());

	const std::vector<ICommandPointer>& commands() const noexcept;
	void addCommand(ICommandPointer commandPointer);
	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;
	virtual bool isCompound() const noexcept;

private:
	const char* m_description;
	QIcon m_icon;
	std::vector<ICommandPointer> m_commands;
};

class OpenUrlCommand : public ICommand
{
public:
	OpenUrlCommand(const QUrl& url);

	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;

private:
	QUrl m_url;
};

class RemoveRowCommand : public ICommand
{
public:
	RemoveRowCommand(int row);

	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;

private:
	int m_row;
};

class ExportDataToXlsxCommand : public ICommand
{
public:
	ExportDataToXlsxCommand(
		const CrawlerEngine::SequencedDataCollection* dataCollection,
		const std::vector<DCStorageDescription>& storageDescriptions
	);

	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;

private:
	const CrawlerEngine::SequencedDataCollection* m_dataCollection;
	std::vector<DCStorageDescription> m_storageDescriptions;
};

class ExportUrlInfoToXlsxCommand : public ICommand
{
public:
	ExportUrlInfoToXlsxCommand(const CrawlerEngine::ISequencedStorage* storage,
		const QVector<ParsedPageInfo::Column>& columns,
		int row
	);

	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;

private:
	const CrawlerEngine::ISequencedStorage* m_storage;
	QVector<ParsedPageInfo::Column> m_availableColumns;
	int m_row;
};


class ExportUrlOutlinksToXlsxCommand : public ICommand
{
public:
	ExportUrlOutlinksToXlsxCommand(const CrawlerEngine::ISequencedStorage* storage, int row);

	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;

private:
	const CrawlerEngine::ISequencedStorage* m_storage;
	int m_row;
};


class ExportUrlInlinksToXlsxCommand : public ICommand
{
public:
	ExportUrlInlinksToXlsxCommand(const CrawlerEngine::ISequencedStorage* storage, int row);

	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;

private:
	const CrawlerEngine::ISequencedStorage* m_storage;
	int m_row;
};


class CopyToClipboardAllPagesCommand : public ICommand
{
public:
	CopyToClipboardAllPagesCommand(const CrawlerEngine::ISequencedStorage* storage);

	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;

private:
	const CrawlerEngine::ISequencedStorage* m_storage;
};

class CopyToClipboardAllColumnsDataCommand : public ICommand
{
public:
	CopyToClipboardAllColumnsDataCommand(
		const CrawlerEngine::ISequencedStorage* storage,
		CrawlerEngine::StorageType storageType,
		int pageIndex
	);

	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;

private:
	const CrawlerEngine::ISequencedStorage* m_storage;
	CrawlerEngine::StorageType m_storageType;
	int m_pageIndex;
};

class CopyToClipboardUrlCommand : public ICommand
{
public:
	CopyToClipboardUrlCommand(
		const CrawlerEngine::ISequencedStorage* storage,
		int pageIndex
	);

	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;

private:
	const CrawlerEngine::ISequencedStorage* m_storage;
	int m_pageIndex;
};

class CheckGoogleCacheCommand : public ICommand
{
public:
	CheckGoogleCacheCommand(const QUrl& url);

	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;

private:
	QUrl m_url;
};

class CheckHTMLWithW3CValidatorCommand : public ICommand
{
public:
	CheckHTMLWithW3CValidatorCommand(const QUrl& url);

	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;

private:
	QUrl m_url;
};

class OpenInWaybackMachineCommand : public ICommand
{
public:
	OpenInWaybackMachineCommand(const QUrl& url);

	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;

private:
	QUrl m_url;
};

class ShowOtherDomainsOnIpCommand : public ICommand
{
public:
	ShowOtherDomainsOnIpCommand(const QByteArray& ipv4);

	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;

private:
	QByteArray m_ipAddress;
};

class OpenRobotsTxtFileCommand : public ICommand
{
public:
	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;
};

class RefreshPageCommand : public ICommand
{
public:
	RefreshPageCommand(CrawlerEngine::StorageType storageType, int index);

	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;
	virtual bool canExecute() const noexcept override;

private:
	CrawlerEngine::StorageType m_storageType;
	int m_index;
};

}