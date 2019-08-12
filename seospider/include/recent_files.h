#pragma once
#include "stdafx.h"

namespace SeoSpider
{

class RecentFiles
{
public:
	static RecentFiles& instance();

	const QVector<QAction*>& recentFilesActions();

	QAction* subMenuAction();

	void registerNewRecentFile(const QString& filePath);
	void updateRecentFileActions();

private:
	RecentFiles();

private:
	QVector<QAction*> m_recentFilesActions;
	QAction* m_placeHolderAction;
};

}