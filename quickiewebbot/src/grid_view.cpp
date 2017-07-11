#include "gridview.h"

namespace QuickieWebBot
{
GridView::GridView(QWidget * parent)
	: QTableView(parent)
{

}
void GridView::paintEvent(QPaintEvent* event)
{
	
	QTableView::paintEvent(event);

}
}