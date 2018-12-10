#pragma once

#include "constants.h"

namespace SeoSpider
{

static QByteArray s_openSerializedFileKey = "open_serialized_file";
static QByteArray s_openSerializedFileDescription = "open " + c_projectFileExtension + " file in program";

static QByteArray s_introducePageKey = "introduce_page";
static QByteArray s_introducePageDescription = "Shows introduce page when starting";

static QByteArray s_startWithoutService = "start_without_service";
static QByteArray s_startWithoutServiceDescription = "This command force starting app without starting seospiderservice app";

static QByteArray s_useOldDownloader = "use_old_downloader";
static QByteArray s_useOldDownloaderDescription = "Force program to use the downloader based on QNetworkAccessManager";

static QByteArray s_workerCount = "worker_count";
static QByteArray s_workerCountDescription = "Sets the parser count";

}
