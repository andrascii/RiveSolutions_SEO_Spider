#pragma once

namespace SeoSpider
{

// action group keys
static const QByteArray s_fileActionGroup = "fileActionGroup";

// file actions
static const QByteArray s_openFileAction = "openFileAction";
static const QByteArray s_closeFileAction = "closeFileAction";
static const QByteArray s_recentFilesAction = "recentFilesAction";
static const QByteArray s_saveFileAction = "saveFileAction";
static const QByteArray s_saveFileAsAction = "saveFileAsAction";

static const QByteArray s_exitProgramAction = "exitProgramAction";

// settings actions
static const QByteArray s_settingsActionGroup = "settingsActionGroup";

static const QByteArray s_openSettingsAction = "openSettingsAction";
static const QByteArray s_openCrawlerSettingsAction = "openCrawlerSettingsAction";
static const QByteArray s_openLanguageSettingsAction = "openLanguageSettingsAction";
static const QByteArray s_openPreferencesSettingsAction = "openPreferencesSettingsAction";
static const QByteArray s_openLimitsSettingsAction = "openLimitsSettingsAction";
static const QByteArray s_openProxySettingsAction = "openProxySettingsAction";

// crawler actions
static const QByteArray s_startCrawlerAction = "startCrawlerAction";
static const QByteArray s_stopCrawlerAction = "stopCrawlerAction";
static const QByteArray s_clearCrawledDataAction = "clearCrawledDataAction";

// sitemap actions
static const QByteArray s_createXMLSitemapAction = "createXMLSitemapAction";

}
