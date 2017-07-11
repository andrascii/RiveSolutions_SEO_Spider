#pragma once

namespace QuickieWebBot
{

class GridView : public QTableView
{
	Q_OBJECT

public:
	explicit GridView(QWidget* parent = nullptr);

protected:
	virtual void paintEvent(QPaintEvent* event) override;
};


}