#pragma once

#include "abstract_page.h"

namespace SeoSpider
{

class TableView;
class PageDataWidget;
class ColumnsLookupLineEditWidget;

class AllPagesPage : public QFrame, public AbstractPage
{
	Q_OBJECT

public:
	AllPagesPage(QWidget* parent = nullptr);
	virtual QWidget* widget() const override;
	virtual QString name() const override;
	virtual QIcon icon() const override;
	virtual Type type() const override;

signals:
	virtual void controlsChanged(IPage::Type) override;

private slots:
	void pageViewSelectionChangedSlot(const QItemSelection& selected, const QItemSelection& deselected);
	void onApplyColumnSearch(int searchKey, const QString& searchValue);
	void onApplyPlainSearch(const QString& searchValue);

private:
	void createHeaderActionWidgets();
	void showNoResultsLabelFor(const QString& searchValue);
	void hideNoResultsLabel();
	void applySearchHelper(int searchColumnNumber, const QString& searchValue);
	TableView* tableView() const;

private:
	enum WidgetType
	{
		NoResultsWidget,
		CrawlingTableViewWidget
	};

	QStackedWidget* m_stackedTableView;
	PageDataWidget* m_pageDataWidget;
	QSplitter* m_splitter;
	ColumnsLookupLineEditWidget* m_lookupLineEditWidget;
	QMap<WidgetType, int> m_widgetIndexes;
};

}