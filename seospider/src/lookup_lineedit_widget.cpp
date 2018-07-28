#include "lookup_lineedit_widget.h"

namespace SeoSpider
{

LookupLineEditWidget::LookupLineEditWidget(QWidget* parent)
	: QFrame(parent)
	, m_toolButtonMenu(new QMenu(this))
	, m_currentSearchKey(-1)
{
	setupUi(this);

	toolButton->setMenu(m_toolButtonMenu);
	toolButton->setPopupMode(QToolButton::InstantPopup);

	VERIFY(connect(lineEdit, &QLineEdit::returnPressed, this, &LookupLineEditWidget::onApplySearch));
	VERIFY(connect(m_toolButtonMenu, &QMenu::triggered, this, &LookupLineEditWidget::onActionTriggered));
}

void LookupLineEditWidget::removeAllSearchFields()
{
	m_toolButtonMenu->clear();
	m_searchFields.clear();
	m_currentSearchKey = -1;
}

void LookupLineEditWidget::addSearchField(int searchKey, const QString& searchKeyDescription)
{
	if (m_searchFields.contains(searchKey))
	{
		WARNLOG << "Specified search key already exists";
		return;
	}

	m_searchFields[searchKey] = searchKeyDescription;

	QAction* action = new QAction(searchKeyDescription);
	action->setData(searchKey);

	m_toolButtonMenu->addAction(action);
}

int LookupLineEditWidget::currentSearchKey() const
{
	return m_currentSearchKey;
}

QString LookupLineEditWidget::currentSearchString() const
{
	return lineEdit->text();
}

void LookupLineEditWidget::onApplySearch()
{
	emit applySearch(m_currentSearchKey, lineEdit->text());
}

void LookupLineEditWidget::onActionTriggered(QAction* action)
{
	m_currentSearchKey = action->data().toInt();
}

}