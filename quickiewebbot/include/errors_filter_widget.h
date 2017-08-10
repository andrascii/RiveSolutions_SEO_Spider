#pragma once

namespace QuickieWebBot
{

class GridView;

class ErrorsFilterWidget : public QWidget
{
public:
	ErrorsFilterWidget(QWidget* parent = nullptr);

private:
	GridView* m_summaryGridView;
	GridView* m_summaryDetailsGridView;
};

}
