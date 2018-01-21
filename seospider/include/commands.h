#pragma once

#include "data_collection_groups_factory.h"

namespace CrawlerEngine
{

class SequencedDataCollection;

}

namespace SeoSpider
{

class ICommand
{
public:
	virtual QIcon icon() const = 0;
	virtual const char* description() const noexcept = 0;
	virtual void execute() = 0;
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
	ExportDataToXlsxCommand(const CrawlerEngine::SequencedDataCollection* dataCollection,
		const std::vector<DCStorageDescription>& storageDescriptions);

	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;

private:
	QString columnLetter(int columnNumber) const;

private:
	const CrawlerEngine::SequencedDataCollection* m_dataCollection;
	std::vector<DCStorageDescription> m_storageDescriptions;
};

}