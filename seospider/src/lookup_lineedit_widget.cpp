#include "lookup_lineedit_widget.h"

namespace SeoSpider
{

LookupLineEditWidget::LookupLineEditWidget(QWidget* parent)
	: QFrame(parent)
	, m_toolButtonMenu(new QMenu(this))
{
	setupUi(this);

	toolButton->setMenu(m_toolButtonMenu);
	toolButton->setPopupMode(QToolButton::InstantPopup);
}

void LookupLineEditWidget::addSearchField(int searchKey, const QString& searchKeyDescription)
{
	if (m_searchFields.contains(searchKey))
	{
		WARNLOG << "Specified search key already exists";
		return;
	}

	m_searchFields[searchKey] = searchKeyDescription;

	m_toolButtonMenu->addAction(new QAction(searchKeyDescription));
}

void LookupLineEditWidget::onSearchKeyChanged(int searchKey)
{
	searchKey;
}

}