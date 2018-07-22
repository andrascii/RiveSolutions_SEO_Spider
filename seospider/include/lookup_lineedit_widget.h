#pragma once

#include "ui_lookup_lineedit_widget.h"

namespace SeoSpider
{

class LookupLineEditWidget : public QFrame, protected Ui::LookupLineEditWidget
{
	Q_OBJECT

public:
	LookupLineEditWidget(QWidget* parent = nullptr);

	void addSearchField(int searchKey, const QString& searchKeyDescription);
	const QString& searchValue() const;

signals:
	void searchKeyChanged(int searchKey);

private slots:
	void onSearchKeyChanged(int searchKey);

private:
	QMap<int, QString> m_searchFields;
	QMenu* m_toolButtonMenu;
};

}