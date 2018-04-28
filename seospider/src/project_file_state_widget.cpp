#include "project_file_state_widget.h"
#include "application.h"
#include "crawler.h"
#include "constants.h"

namespace SeoSpider
{

ProjectFileStateWidget::ProjectFileStateWidget(QWidget* parent)
	: QWidget(parent)
	, m_projectFileNameLabel(new QLabel(m_projectFileName, this))
	, m_stateDescriptor(std::pair(false, CrawlerEngine::Session::StateNone))
{
	VERIFY(connect(theApp->crawler(), &CrawlerEngine::Crawler::sessionCreated, this, &ProjectFileStateWidget::show));
	VERIFY(connect(theApp->crawler(), &CrawlerEngine::Crawler::sessionCreated, this, &ProjectFileStateWidget::onSessionStateCreated));
	VERIFY(connect(theApp->crawler(), &CrawlerEngine::Crawler::sessionDestroyed, this, &ProjectFileStateWidget::hide));
	VERIFY(connect(theApp->crawler(), &CrawlerEngine::Crawler::sessionDestroyed, this, &ProjectFileStateWidget::onSessionStateDestroyed));
	VERIFY(connect(theApp->crawler(), &CrawlerEngine::Crawler::sessionStateChanged, this, &ProjectFileStateWidget::onSessionStateChanged));
	VERIFY(connect(theApp->crawler(), &CrawlerEngine::Crawler::sessionNameChanged, this, &ProjectFileStateWidget::setProjectFileName));

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(m_projectFileNameLabel);

	hide();
}

void ProjectFileStateWidget::setProjectFileName(const QString& name)
{
	if (!m_stateDescriptor.first)
	{
		return;
	}

	QFileInfo fileInfo(name);

	m_projectFileName = fileInfo.baseName() + c_projectFileExtension;

	setNameHelper();
}

void ProjectFileStateWidget::onSessionStateChanged(CrawlerEngine::Session::State state)
{
	m_stateDescriptor.second = state;

	setNameHelper();
}

void ProjectFileStateWidget::onSessionStateCreated()
{
	m_stateDescriptor = std::pair(true, theApp->crawler()->sessionState());

	setProjectFileName(theApp->crawler()->sessionName());
}

void ProjectFileStateWidget::onSessionStateDestroyed()
{
	m_stateDescriptor = std::pair(false, CrawlerEngine::Session::StateNone);
}

void ProjectFileStateWidget::setNameHelper()
{
	if (m_stateDescriptor.second == CrawlerEngine::Session::StateUnsaved)
	{
		m_projectFileNameLabel->setText(m_projectFileName + "*");
	}
	else
	{
		m_projectFileNameLabel->setText(m_projectFileName);
	}
}

}