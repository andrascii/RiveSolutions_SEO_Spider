#pragma once

#include "parsed_page.h"

namespace CrawlerEngine
{

class RobotsTxtTokenizer;

class MetaRobotsHelpers
{
public:
	static UserAgentType userAgent(const QString& userAgentStr);
	static QStringList supportedUserAgents(bool metaRobots = false);

	static bool checkIfSupportedRecordExistsAndCorrectUserAgentType(UserAgentType& userAgentType, const RobotsTxtTokenizer& tokenizer);
	static bool checkIfSupportedMetaRobotsExistAndCorrectUserAgentType(UserAgentType& userAgentType, const MetaRobotsFlagsSet& metaRobotsFlags);
};

}