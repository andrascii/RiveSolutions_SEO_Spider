#include "stdafx.h"
#include "menubar.h"
#include "action_registry.h"
#include "action_keys.h"

namespace SeoSpider
{

MenuBar::MenuBar(QWidget* parent)
	: QMenuBar(parent)
{
	init();
}

void MenuBar::init()
{
	addMenu(buildMenuFile());
	addMenu(buildMenuSettings());
	addMenu(buildMenuSitemap());
	addMenu(buildMenuHelp());
}

QMenu* MenuBar::buildMenuFile()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	QMenu* fileMenu = new QMenu(tr("File"), this);

#ifdef SUPPORT_SERIALIZATION
	fileMenu->addAction(actionRegistry.globalAction(s_openFileAction));
	fileMenu->addAction(actionRegistry.globalAction(s_recentFilesAction));
	fileMenu->addSeparator();
	fileMenu->addAction(actionRegistry.globalAction(s_saveFileAction));
	fileMenu->addAction(actionRegistry.globalAction(s_saveFileAsAction));
	fileMenu->addAction(actionRegistry.globalAction(s_closeFileAction));
	fileMenu->addSeparator();
#endif
	fileMenu->addAction(actionRegistry.globalAction(s_exitProgramAction));

	return fileMenu;
}

QMenu* MenuBar::buildMenuSettings()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();
	QMenu* settingsMenu = new QMenu(tr("Settings"), this);

	settingsMenu->addAction(actionRegistry.globalAction(s_openCrawlerSettingsAction));
	settingsMenu->addAction(actionRegistry.globalAction(s_openLanguageSettingsAction));
	settingsMenu->addAction(actionRegistry.globalAction(s_openPreferencesSettingsAction));
	settingsMenu->addAction(actionRegistry.globalAction(s_openLimitsSettingsAction));
	settingsMenu->addAction(actionRegistry.globalAction(s_openProxySettingsAction));
	settingsMenu->addAction(actionRegistry.globalAction(s_openUserAgentSettingsAction));
	settingsMenu->addAction(actionRegistry.globalAction(s_openCrawlerPauseTimerSettingsAction));
	settingsMenu->addAction(actionRegistry.globalAction(s_openCompanyProfileSettingsAction));

#ifndef PRODUCTION
	settingsMenu->addAction(actionRegistry.globalAction(s_openPageVisualSettingsAction));
#endif

	settingsMenu->addAction(actionRegistry.globalAction(s_yandexMetricaSettingsAction));
	settingsMenu->addAction(actionRegistry.globalAction(s_googleAnalyticsSettingsAction));

	return settingsMenu;
}

QMenu* MenuBar::buildMenuSitemap()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();
	QMenu* sitemapMenu = new QMenu(tr("Sitemap"), this);

	sitemapMenu->addAction(actionRegistry.globalAction(s_createXMLSitemapAction));

	return sitemapMenu;
}

QMenu* MenuBar::buildMenuHelp()
{
	ActionRegistry& actionRegistry = ActionRegistry::instance();

	QMenu* helpMenu = new QMenu(tr("Help"), this);

	helpMenu->addAction(actionRegistry.globalAction(s_showHelpAction));
	helpMenu->addAction(actionRegistry.globalAction(s_sendFeedbackAction));
	helpMenu->addSeparator();
#ifdef CHECK_LICENSE
	helpMenu->addAction(actionRegistry.globalAction(s_registerProductAction));
#endif
	helpMenu->addAction(actionRegistry.globalAction(s_checkForUpdatesAction));
	helpMenu->addSeparator();
	helpMenu->addAction(actionRegistry.globalAction(s_aboutProductAction));

	return helpMenu;
}

}