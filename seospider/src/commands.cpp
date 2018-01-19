#include "commands.h"

namespace SeoSpider
{

OpenUrlCommand::OpenUrlCommand(const QUrl& url)
	: m_url(url)
{
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

//////////////////////////////////////////////////////////////////////////

RemoveRowCommand::RemoveRowCommand(int row)
	: m_row(row)
{
	ASSERT(m_row > 0);
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

}