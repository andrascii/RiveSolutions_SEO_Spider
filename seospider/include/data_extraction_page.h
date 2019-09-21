#pragma once

#include "abstract_table_page.h"

namespace SeoSpider
{

class TableView;
class PageDataWidget;
class LookupLineEditWidget;
class ColumnsLookupLineEditWidget;

class DataExtractionPage : public AbstractTablePage
{
    Q_OBJECT

public:
    DataExtractionPage(QWidget* parent = nullptr);
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
    void onResourceTypeFilterChanged(int filter);

private:
    void createHeaderActionWidgets();
    void showNoResultsLabelFor(const QString& searchValue);
    void hideNoResultsLabel();
    void applySearchHelper(int searchColumnNumber, const QString& searchValue);
    TableView* tableView() const;

protected:
    virtual void showEvent(QShowEvent* event);

private:
    enum WidgetType
    {
        NoResultsWidget,
        CrawlingTableViewWidget
    };

    QStackedWidget* m_stackedTableView;
    QSplitter* m_splitter;
    ColumnsLookupLineEditWidget* m_columnsLookupLineEditWidget;
    LookupLineEditWidget* m_lookupLineEditWidget;
    QMap<WidgetType, int> m_widgetIndexes;
    QAction* m_exportFilterDataAction;
};

}