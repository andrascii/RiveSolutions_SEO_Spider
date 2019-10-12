#pragma once

namespace SeoSpider
{

class RecentFiles
{
public:
	static RecentFiles& instance();

	const QVector<QAction*>& recentFilesActions();

	QAction* subMenuAction();

	void forgetRecentFile(const QString& filePath);
	void registerNewRecentFile(const QString& filePath);
	void updateRecentFileActions();

private:
	RecentFiles();

private:
	QVector<QAction*> m_recentFilesActions;
	QAction* m_placeHolderAction;
};

}