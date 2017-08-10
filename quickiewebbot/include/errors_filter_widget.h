#pragma once

namespace QuickieWebBot
{

class GridView;

class ErrorsFilterWidget : public QWidget
{
	Q_OBJECT

public:
	ErrorsFilterWidget(QWidget* parent = nullptr);

private:
	Q_SLOT void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
	GridView* m_summaryGridView;
	GridView* m_summaryDetailsGridView;
};

}
