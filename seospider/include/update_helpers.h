#pragma once

namespace SeoSpider
{

class UpdateHelpers
{
public:
	static constexpr const char* actualVersionFileUrl()
	{
		//return "http://rivesolutions.com/actual_version.txt";
		return "http://rivesolutions.com/Updates.xml";
	}

	static constexpr const char* downloadAddressFileUrl()
	{
		return "http://rivesolutions.com/download_address.txt";
	}

	static constexpr const char* updatePatchSavePathKey()
	{
		return "updatePatchSavePath";
	}

	static QString updatesPatchSaveDirPath();
};

}