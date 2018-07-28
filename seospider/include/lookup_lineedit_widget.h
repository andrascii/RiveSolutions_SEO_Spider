#pragma once

#include "ui_lookup_lineedit_widget.h"

namespace SeoSpider
{

class LookupLineEditWidget : public QFrame, protected Ui::LookupLineEditWidget
{
	Q_OBJECT

public:
	LookupLineEditWidget(QWidget* parent = nullptr);

	void removeAllSearchFields();
	void addSearchField(int searchKey, const QString& searchKeyDescription);
	int currentSearchKey() const;
	QString currentSearchString() const;

signals:
	void applySearch(int searchKey, const QString& searchString);

private slots:
	void onApplySearch();
	void onActionTriggered(QAction* action);

private:
	QMap<int, QString> m_searchFields;
	QMenu* m_toolButtonMenu;
	int m_currentSearchKey;
};

}