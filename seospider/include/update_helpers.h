#pragma once

namespace SeoSpider
{

class UpdateHelpers
{
public:
	static constexpr const char* actualVersionFileUrl()
	{
		return "http://rivesolutions.com/Updates.xml";
	}

	static constexpr const char* localVersionXmlFile()
	{
		return "components.xml";
	}
};

}