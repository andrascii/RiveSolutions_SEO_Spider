#include "abstract_page.h"
#include "header_toolbutton_creator.h"
#include "application.h"
#include "main_window.h"
#include "content_frame.h"

namespace SeoSpider
{

const QString AbstractPage::s_noResultsMessageStub(QObject::tr("No results found for \"%1\""));

QList<QWidget*> AbstractPage::bindControls() const
{
	return m_controls;
}

void AbstractPage::addWidget(QWidget* control, int position)
{
	m_controls.removeOne(control);

	if (position == -1)
	{
		m_controls.push_back(control);
	}
	else
	{
		m_controls.insert(position, control);
	}

	if (!m_controls.contains(control))
	{
		control->hide();
	}

	emit controlsChanged(type());
}

void AbstractPage::addAction(QAction* action, int position)
{
	// TODO: use position
	Q_UNUSED(position);
	ASSERT(action != nullptr);

	QWidget* widget = m_controlsByAction.value(action, nullptr);

	if (widget == nullptr)
	{
		widget = createControl(action);
	}

	addWidget(widget, position);
}

void AbstractPage::showMe() const
{
	theApp->mainWindow()->contentFrame()->showPage(type());
}

void AbstractPage::goTo(PageDataWidget::PageDataType pageDataType)
{
	if (m_pageDataWidget != nullptr)
	{
		m_pageDataWidget->selectTab(pageDataType);
	}
}

QWidget* AbstractPage::createControl(QAction* action) const
{
	return HeaderToolButtonCreator::createControl(action);
}

}