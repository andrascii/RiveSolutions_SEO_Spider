#include "page_data_widget_splitter.h"
#include "helpers.h"
#include "page_data_widget.h"

namespace SeoSpider
{

namespace {

class PageDataWidgetSplitterSyncronizer
{
public:
	void registerSplitter(PageDataWidgetSplitter* splitter)
	{
		m_splitters.append(splitter);
	}

	void unRegisterSplitter(PageDataWidgetSplitter* splitter)
	{
		m_splitters.removeAll(splitter);
	}

	void syncSplitter(PageDataWidgetSplitter* sourceSplitter)
	{
		if (m_syncActive)
		{
			return;
		}

		m_syncActive = true;

		for (PageDataWidgetSplitter* splitter : m_splitters)
		{
			splitter->syncronize(sourceSplitter);
		}

		m_syncActive = false;
	}

private:
	QList<PageDataWidgetSplitter*> m_splitters;
	bool m_syncActive = false;
};

PageDataWidgetSplitterSyncronizer splitterSyncronizer;

}
	
PageDataWidgetSplitter::PageDataWidgetSplitter(QWidget* parent, QWidget* splitterFirstWidget,
	PageDataWidget* splitterSecondWidget)
	: QSplitter(parent)
	, m_splitterFirstWidget(splitterFirstWidget)
	, m_splitterSecondWidget(splitterSecondWidget)
	, m_wasShown(false)
	, m_pageDataWidgetCollapsed(false)
	, m_animationEndHeight(0)
	, m_defaultTableViewHeight(Common::Helpers::pointsToPixels(400))
{
	setOrientation(Qt::Vertical);
	setChildrenCollapsible(false);
	addWidget(splitterFirstWidget);

	if (splitterSecondWidget)
	{
		addPageDataWidget(splitterSecondWidget);
	}

	splitterSyncronizer.registerSplitter(this);
}

PageDataWidgetSplitter::~PageDataWidgetSplitter()
{
	splitterSyncronizer.unRegisterSplitter(this);
}

void PageDataWidgetSplitter::addPageDataWidget(PageDataWidget* splitterSecondWidget)
{
	m_splitterSecondWidget = splitterSecondWidget;
	addWidget(splitterSecondWidget);

	VERIFY(connect(splitterSecondWidget, &PageDataWidget::tabBarClicked, this, &PageDataWidgetSplitter::onPageWidgetTabClicked));
	splitterSecondWidget->installEventFilter(this);
}

int PageDataWidgetSplitter::secondWidgetHeight() const
{
	return m_splitterSecondWidget != nullptr ? m_splitterSecondWidget->height() : 0;
}

void PageDataWidgetSplitter::setSecondWidgetHeight(int height)
{
	if (m_splitterSecondWidget != nullptr)
	{
		setSecondWidgetHeightImpl(height, true);
	}
}

void PageDataWidgetSplitter::syncronize(const PageDataWidgetSplitter* other)
{
	if (other == this)
	{
		return;
	}

	if (other->m_splitterSecondWidget == nullptr || m_splitterSecondWidget == nullptr)
	{
		return;
	}

	if (other->m_splitterSecondWidget->height() == m_splitterSecondWidget->height())
	{
		return;
	}
	
	setSecondWidgetHeightImpl(other->m_splitterSecondWidget->height(), false);
	m_splitterSecondWidget->setMaximumHeight(other->m_splitterSecondWidget->maximumHeight());
	m_defaultTableViewHeight = other->m_splitterFirstWidget->height();

	m_pageDataWidgetCollapsed = other->m_pageDataWidgetCollapsed;
	m_prevDatawidgetHeight = other->m_prevDatawidgetHeight;
	m_animationEndHeight = other->m_animationEndHeight;
}

bool PageDataWidgetSplitter::eventFilter(QObject* watched, QEvent* event)
{
	if (m_animationEndHeight != 0 || event->type() != QEvent::Resize)
	{
		return QSplitter::eventFilter(watched, event);
	}

	splitterSyncronizer.syncSplitter(this);
	return QSplitter::eventFilter(watched, event);
}

void PageDataWidgetSplitter::showEvent(QShowEvent*)
{
	if (m_wasShown)
	{
		return;
	}

	QWidget* parentWidget = qobject_cast<QWidget*>(parent())->parentWidget();

	ASSERT(parentWidget);

	const int parentWidgetHeight = parentWidget->height();
	const int mainTableView = m_defaultTableViewHeight;

	setSizes(QList<int>() << mainTableView << parentWidgetHeight - mainTableView);
	m_wasShown = true;
}

int PageDataWidgetSplitter::collapsedPageDataWidgetHeight()
{
	static int s_collapsedPageDataWidgetHeight = Common::Helpers::pointsToPixels(22);
	return s_collapsedPageDataWidgetHeight;
}

void PageDataWidgetSplitter::setSecondWidgetHeightImpl(int height, bool setMaximumHeight)
{
	const bool animationEnd = height == m_animationEndHeight &&
		m_splitterSecondWidget->height() != height;
	
	int parentWidgetHeight = m_splitterFirstWidget->height() + m_splitterSecondWidget->height();

	if (!m_wasShown)
	{
		QWidget* parentWidget = qobject_cast<QWidget*>(parent())->parentWidget();
		ASSERT(parentWidget);
		parentWidgetHeight = parentWidget->height();
	}

	if (setMaximumHeight)
	{
		m_splitterSecondWidget->setMaximumHeight(height);
	}

	setSizes(QList<int>() << parentWidgetHeight - height << height);

	if (animationEnd)
	{
		if (height != collapsedPageDataWidgetHeight())
		{
			m_splitterSecondWidget->setMaximumHeight(100000);
			m_pageDataWidgetCollapsed = false;
		}
		else
		{
			m_pageDataWidgetCollapsed = true;
		}
		m_animationEndHeight = 0;
		splitterSyncronizer.syncSplitter(this);
	}
}

	void PageDataWidgetSplitter::onPageWidgetTabClicked(int index, int prevIndex)
{
	if (m_animationEndHeight != 0)
	{
		return;
	}

	if (index != prevIndex && !m_pageDataWidgetCollapsed)
	{
		return;
	}

	QWidget* parentWidget = qobject_cast<QWidget*>(parent())->parentWidget();

	ASSERT(parentWidget);
	const int parentWidgetHeight = parentWidget->height();

	QPropertyAnimation* animation = new QPropertyAnimation(this, "secondWidgetHeight");
	animation->setEasingCurve(QEasingCurve::OutQuint);
	animation->setDuration(200);

	if (index == prevIndex && !m_pageDataWidgetCollapsed)
	{
		m_prevDatawidgetHeight = m_splitterSecondWidget->height() / static_cast<double>(parentWidgetHeight);
		animation->setStartValue(m_splitterSecondWidget->height());
		m_animationEndHeight = collapsedPageDataWidgetHeight();
		animation->setEndValue(m_animationEndHeight);
	}
	else
	{
		animation->setStartValue(m_splitterSecondWidget->height());
		m_animationEndHeight = parentWidgetHeight * m_prevDatawidgetHeight;
		animation->setEndValue(m_animationEndHeight);
	}

	animation->start();
}

}
