#pragma once

namespace SeoSpider
{

class UpdateHelpers
{
public:
	static constexpr const char* actualVersionFileUrl()
	{
		return "http://rivesolutions.com/actual_version.txt";
	}

	static constexpr const char* downloadAddressFileUrl()
	{
		return "http://rivesolutions.com/download_address.txt";
	}

	static QString updatesPatchSaveDirPath();
};

}