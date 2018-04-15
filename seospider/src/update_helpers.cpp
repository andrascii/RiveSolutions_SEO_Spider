#include "update_helpers.h"

namespace SeoSpider
{

QString UpdateHelpers::updatesPatchSaveDirPath()
{
	QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	path = QDir::cleanPath(path + QString("/RiveSolutions/SeoSpider/Update/"));

	QDir dir(path);

	if (!dir.exists())
	{
		dir.mkpath(path);
	}

	return path;
}

}