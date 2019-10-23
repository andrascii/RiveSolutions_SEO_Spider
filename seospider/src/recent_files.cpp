#include "stdafx.h"
#include "recent_files.h"
#include "application.h"
#include "main_window.h"

namespace
{

const int c_maxRecentFiles = 10;

}

namespace SeoSpider
{

RecentFiles::RecentFiles()
	: m_placeHolderAction(new QAction(QObject::tr("There is no recent files yet")))
{
	m_placeHolderAction->setVisible(true);
	m_placeHolderAction->setEnabled(false);

	for (int i = 0; i < c_maxRecentFiles; i++)
	{
		m_recentFilesActions.push_back(new QAction);
		m_recentFilesActions.back()->setVisible(false);

		VERIFY(QObject::connect(m_recentFilesActions.back(), SIGNAL(triggered()),
			theApp->mainWindow(), SLOT(openRecentFile())));
	}

	updateRecentFileActions();
}

const QVector<QAction*>& RecentFiles::recentFilesActions()
{
	return m_recentFilesActions;
}

QAction* RecentFiles::subMenuAction()
{
	QMenu* recentFilesMenu = new QMenu;

	recentFilesMenu->addAction(m_placeHolderAction);

	foreach(auto action, recentFilesActions())
	{
		recentFilesMenu->addAction(action);
	}

	return recentFilesMenu->menuAction();
}

void RecentFiles::forgetRecentFile(const QString& filePath)
{
	QStringList recentFilesList = theApp->loadFromSettings("recentFilesList").toStringList();
	recentFilesList.removeAll(filePath);
	theApp->saveToSettings("recentFilesList", recentFilesList);
	updateRecentFileActions();
}

void RecentFiles::registerNewRecentFile(const QString& filePath)
{
	QStringList recentFilesList = theApp->loadFromSettings("recentFilesList").toStringList();

	recentFilesList.removeAll(filePath);
	recentFilesList.prepend(filePath);

	while (recentFilesList.size() > c_maxRecentFiles)
	{
		recentFilesList.removeLast();
	}

	theApp->saveToSettings("recentFilesList", recentFilesList);

	updateRecentFileActions();
}

void RecentFiles::updateRecentFileActions()
{
	QStringList recentFilesList = theApp->loadFromSettings("recentFilesList").toStringList();

	const int recentFilesCount = qMin(recentFilesList.size(), c_maxRecentFiles);

	for(int i = 0; i < recentFilesCount; ++i)
	{
		const QString text = QObject::tr("&%1 %2").arg(i + 1).arg(QFileInfo(recentFilesList[i]).fileName());

		m_recentFilesActions[i]->setText(text);
		m_recentFilesActions[i]->setData(recentFilesList[i]);
		m_recentFilesActions[i]->setVisible(true);
	}

	for(int i = recentFilesCount; i < c_maxRecentFiles; ++i)
	{
		m_recentFilesActions[i]->setVisible(false);
	}

	m_placeHolderAction->setVisible(!recentFilesCount);
}

RecentFiles& RecentFiles::instance()
{
	static RecentFiles recentFiles;

	return recentFiles;
}

}
