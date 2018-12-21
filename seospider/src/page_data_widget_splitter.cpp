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
	, m_pageDataWidgetCollapsed(false)
	, m_animationEndHeight(0)
	, m_firstShowEvent(true)
	, m_prevIndex(0)
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
	VERIFY(connect(splitterSecondWidget, &PageDataWidget::tabSelected, this, &PageDataWidgetSplitter::onPageWidgetTabSelected));
	VERIFY(connect(splitterSecondWidget->tabWidget(), &QTabBar::currentChanged, this, &PageDataWidgetSplitter::updateTabTooltips));
	splitterSecondWidget->installEventFilter(this);
	updateTabTooltips();

	m_splitterSecondWidget->setMaximumHeight(collapsedPageDataWidgetHeight());
	m_prevDatawidgetHeight = 0.3;
	m_pageDataWidgetCollapsed = true;
	m_splitterFirstWidget->adjustSize();
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

	const int height = other->m_splitterSecondWidget->height();
	setSecondWidgetHeightImpl(height, false);
	m_splitterSecondWidget->setMaximumHeight(other->m_splitterSecondWidget->maximumHeight());

	m_pageDataWidgetCollapsed = other->m_pageDataWidgetCollapsed;
	m_prevDatawidgetHeight = other->m_prevDatawidgetHeight;
	m_animationEndHeight = other->m_animationEndHeight;
	m_secondWidgetHeight = height;
	updateTabTooltips();
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
	if (m_secondWidgetHeight != std::nullopt && m_firstShowEvent)
	{
		setSecondWidgetHeightImpl(m_secondWidgetHeight.value(), false);
		m_firstShowEvent = false;
	}
}

int PageDataWidgetSplitter::collapsedPageDataWidgetHeight()
{
	static int s_collapsedPageDataWidgetHeight = Common::Helpers::pointsToPixels(24);
	return s_collapsedPageDataWidgetHeight;
}

void PageDataWidgetSplitter::setSecondWidgetHeightImpl(int height, bool setMaximumHeight)
{
	const bool animationEnd = height == m_animationEndHeight &&
		m_splitterSecondWidget->height() != height;

	const int parentWidgetHeight = m_splitterFirstWidget->height() + m_splitterSecondWidget->height();

	if (setMaximumHeight)
	{
		m_splitterSecondWidget->setMaximumHeight(height);
	}

	const int firstWidgetHeight = parentWidgetHeight - height;

	setSizes(QList<int>() << firstWidgetHeight << height);

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
		updateTabTooltips();
	}
}

void PageDataWidgetSplitter::updateTabTooltips()
{
	for (int i = 0; i < m_splitterSecondWidget->tabWidget()->count(); ++i)
	{
		QTabBar* tabWidget = m_splitterSecondWidget->tabWidget();

		if (m_pageDataWidgetCollapsed)
		{
			tabWidget->setTabToolTip(i, tr("Show Page Information"));
		}
		else if (tabWidget->currentIndex() != i)
		{
			tabWidget->setTabToolTip(i, tabWidget->tabText(i));
		}
		else
		{
			tabWidget->setTabToolTip(i, tr("Hide Page Information"));
		}
	}
}

void PageDataWidgetSplitter::onPageWidgetTabClicked(int index)
{
	if (m_animationEndHeight != 0)
	{
		m_prevIndex = index;
		return;
	}

	if (index != m_prevIndex && !m_pageDataWidgetCollapsed)
	{
		m_prevIndex = index;
		return;
	}

	QWidget* parentWidget = qobject_cast<QWidget*>(parent())->parentWidget();

	ASSERT(parentWidget);
	const int parentWidgetHeight = parentWidget->height();

	QPropertyAnimation* animation = new QPropertyAnimation(this, "secondWidgetHeight");
	animation->setEasingCurve(QEasingCurve::OutQuint);
	animation->setDuration(200);

	if (index == m_prevIndex && !m_pageDataWidgetCollapsed)
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

	m_prevIndex = index;
	animation->start();
}

void PageDataWidgetSplitter::onPageWidgetTabSelected(int index)
{
	m_prevIndex = -1;
	onPageWidgetTabClicked(index);
}
}
