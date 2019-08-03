#pragma once
#include "stdafx.h"

#include "session.h"

namespace SeoSpider
{

class ProjectFileStateWidget : public QWidget
{
	Q_OBJECT

public:
	ProjectFileStateWidget(QWidget* parent = nullptr);

public slots:
	void setProjectFileName(const QString& name);

private slots:
	void onSessionStateChanged(CrawlerEngine::Session::State state);
	void onSessionStateCreated();
	void onSessionStateDestroyed();

private:
	void setNameHelper();

private:
	QString m_projectFileName;
	QLabel* m_projectFileNameLabel;
	std::pair<bool, CrawlerEngine::Session::State> m_stateDescriptor;
};

}