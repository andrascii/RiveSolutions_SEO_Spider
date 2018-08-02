#include "columns_lookup_lineedit_widget.h"
#include "svg_renderer.h"
#include "statistic_counter.h"

namespace SeoSpider
{

ColumnsLookupLineEditWidget::ColumnsLookupLineEditWidget(QWidget* parent)
	: QFrame(parent)
	, m_actionGroup(nullptr)
	, m_toolButtonMenu(new QMenu(this))
	, m_currentSearchKey(-1)
	, m_counter(new StatisticCounter(QStringLiteral("ColumnsLookupLineEditWidgetApplySearchCounter"), this))
{
	setupUi(this);

	toolButton->setMenu(m_toolButtonMenu);
	toolButton->setPopupMode(QToolButton::InstantPopup);
	toolButton->setIcon(SvgRenderer::render(QStringLiteral(":/images/lookup-widget.svg"), 20, 20));
	toolButton->setEnabled(false);

	VERIFY(connect(lineEdit, &QLineEdit::returnPressed, this, &ColumnsLookupLineEditWidget::onApplySearch));
	VERIFY(connect(m_toolButtonMenu, &QMenu::triggered, this, &ColumnsLookupLineEditWidget::onActionTriggered));

	configureLineEdit();
	configureToolButton();
}

void ColumnsLookupLineEditWidget::reset()
{
	m_toolButtonMenu->clear();
	m_searchFields.clear();
	m_currentSearchKey = -1;

	m_actionGroup->deleteLater();
	m_actionGroup = nullptr;

	lineEdit->clear();

	configureLineEdit();
	configureToolButton();
}

void ColumnsLookupLineEditWidget::addSearchField(int searchKey, const QString& searchKeyDescription)
{
	if (std::find(m_searchFields.begin(), m_searchFields.end(), searchKey) != m_searchFields.end())
	{
		WARNLOG << "Specified search key already exists";
		return;
	}

	if (!m_actionGroup)
	{
		m_actionGroup = new QActionGroup(this);
		m_actionGroup->setExclusive(true);
	}

	QAction* action = new QAction(searchKeyDescription);
	action->setData(searchKey);
	action->setCheckable(true);
	m_actionGroup->addAction(action);

	m_toolButtonMenu->addAction(action);
	m_searchFields.push_back(searchKey);

	configureToolButton();
}

int ColumnsLookupLineEditWidget::currentSearchKey() const
{
	return m_currentSearchKey;
}

QString ColumnsLookupLineEditWidget::currentSearchString() const
{
	return lineEdit->text();
}

void ColumnsLookupLineEditWidget::onApplySearch()
{
	emit applySearch(m_currentSearchKey, lineEdit->text());

	m_counter->increment();
}

void ColumnsLookupLineEditWidget::onActionTriggered(QAction* action)
{
	m_currentSearchKey = action->data().toInt();
	const QString placeholderText = tr("Search by \"") + action->text() + tr("\" column");

	configureLineEdit(placeholderText);
}

void ColumnsLookupLineEditWidget::configureLineEdit(const QString& placeholderText)
{
	const bool hasCheckedAction = m_actionGroup && m_actionGroup->checkedAction() != nullptr;

	lineEdit->setEnabled(hasCheckedAction);
	lineEdit->setPlaceholderText(hasCheckedAction ? placeholderText : tr("Select the column for search..."));
}

void ColumnsLookupLineEditWidget::configureToolButton()
{
	toolButton->setEnabled(!m_searchFields.empty());
}

QAction* ColumnsLookupLineEditWidget::actionBySearchKey(int searchKey) const
{
	for (const auto& action : m_actionGroup->actions())
	{
		if (action->data().toInt() == searchKey)
		{
			return action;
		}
	}

	return nullptr;
}

void ColumnsLookupLineEditWidget::setCurrentSearchData(int searchKey, const QString& searchData)
{
	m_currentSearchKey = searchKey;
	lineEdit->setText(searchData);

	QAction* action = actionBySearchKey(m_currentSearchKey);
	action->trigger();
}

}