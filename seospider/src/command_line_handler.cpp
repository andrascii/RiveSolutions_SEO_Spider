#include "stdafx.h"
#include "command_line_handler.h"
#include "command_line_keys.h"
#include "application.h"
#include "seo_spider_service_api_loader.h"

namespace SeoSpider
{

CommandLineHandler::CommandLineHandler(int& argc, char** argv)
{
	m_optionsDescription.add_options()
#ifdef SUPPORT_SERIALIZATION
		(s_openSerializedFileKey, boost::program_options::value<std::string>(), s_openSerializedFileDescription)
#endif
		(s_introducePageKey, s_introducePageDescription)
		(s_startWithoutService, s_startWithoutServiceDescription)
		(s_useOldDownloader, s_useOldDownloaderDescription)
		(s_workerCount, boost::program_options::value<std::string>(), s_workerCountDescription);

	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, m_optionsDescription), m_variablesMap);
	boost::program_options::notify(m_variablesMap);

#ifdef QT_DEBUG
	if (hasCommand(s_startWithoutService))
	{
		// we must do this here because on the next initialization steps
		// we call some service functions which lead us to the loading of this
		// library and starting service but using this key we don't wanna start the service
		Common::SeoSpiderServiceApiLoader::disableService();
	}
#endif
}

QString CommandLineHandler::commandValue(const char* commandKey) const
{
	if (m_variablesMap.count(commandKey))
	{
		return QString(m_variablesMap[commandKey].as<std::string>().c_str());
	}

	return QString();
}


bool CommandLineHandler::hasCommand(const char* commandKey) const
{
	return m_variablesMap.count(commandKey) != 0;
}

}
