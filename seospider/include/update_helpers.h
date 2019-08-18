#pragma once

namespace SeoSpider
{

class UpdateHelpers
{
public:
	static constexpr const char* actualVersionFileUrl()
	{
	#ifdef Q_OS_WIN64
		return "http://rivesolutions.com/downloads/win/x64/Updates.xml";
	#else
		return "http://rivesolutions.com/downloads/win/x86/Updates.xml";
	#endif
	}

	static constexpr const char* localVersionXmlFile()
	{
		return "components.xml";
	}
};

}