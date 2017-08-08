#include "errors_filter_widget.h"
#include "summary_model.h"
#include "summary_view_model.h"

namespace QuickieWebBot
{

ErrorsFilterWidget::ErrorsFilterWidget(QWidget* parent)
	: QWidget(parent)
{
	init();
}

void ErrorsFilterWidget::init()
{
	setupUi(this);

	SummaryModel* summaryModel = new SummaryModel(this);
	SummaryViewModel* summaryViewModel = new SummaryViewModel(summaryModel, this);

	summaryGridView->setModel(summaryModel);
	summaryGridView->setViewModel(summaryViewModel);
}

}