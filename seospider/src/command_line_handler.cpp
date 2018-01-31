#include "command_line_handler.h"
#include "command_line_keys.h"
#include "application.h"

namespace SeoSpider
{

SeoSpider::CommandLineHandler::CommandLineHandler(int& argc, char** argv)
{
	m_optionsDescription.add_options()
		(s_openSerializedFileKey, boost::program_options::value<std::string>(), s_openSerializedFileDescription)
		(s_introducePageKey, s_introducePageDescription);

	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, m_optionsDescription), m_variablesMap);
	boost::program_options::notify(m_variablesMap);
}

QString CommandLineHandler::getCommandArguments(const char* commandKey)
{
	if (m_variablesMap.count(commandKey))
	{
		return QString(m_variablesMap[commandKey].as<std::string>().c_str());
	}
	return QString();
}

}
