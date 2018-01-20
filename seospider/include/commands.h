#pragma once

namespace SeoSpider
{

class ICommand
{
public:
	enum CommandType
	{
		OpenUrlCommandType,
		RefreshAllResultsCommandType,
		RemoveRowCommandType,
		CheckIndexCommandType,
		CheckGoogleCommandType,
		OpenRobotsTxtCommandType,
		ShowOtherDomainsOnIpCommandType,
		CheckHtmlCommandType,
		CopyToClipboardAllColumnsCommandType,
		CopyToClipboardAllPagesCommandType,
		CopyToClipboardUrlCommandType
	};

	virtual QIcon icon() const = 0;
	virtual const char* description() const noexcept = 0;
	virtual void execute() = 0;
	virtual CommandType type() const noexcept = 0;
};

using ICommandPointer = std::shared_ptr<ICommand>;

class OpenUrlCommand : public ICommand
{
public:
	OpenUrlCommand(const QUrl& url);

	virtual QIcon icon() const override;
	virtual const char* description() const noexcept override;
	virtual void execute() override;
	virtual CommandType type() const noexcept override;

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
	virtual CommandType type() const noexcept override;

private:
	int m_row;
};

}