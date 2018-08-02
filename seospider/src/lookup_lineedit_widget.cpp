#include "lookup_lineedit_widget.h"
#include "svg_renderer.h"
#include "statistic_counter.h"

namespace SeoSpider
{

LookupLineEditWidget::LookupLineEditWidget(QWidget* parent)
	: QFrame(parent)
	, m_counter(new StatisticCounter(QStringLiteral("LookupLineEditWidgetApplySearchCounter"), this))
{
	setupUi(this);

	clearButton->hide();

	VERIFY(connect(applyButton, &QPushButton::clicked, this, &LookupLineEditWidget::onApplySearch));
	VERIFY(connect(lineEdit, &QLineEdit::returnPressed, this, &LookupLineEditWidget::onApplySearch));
	VERIFY(connect(lineEdit, SIGNAL(textChanged(const QString&)), SLOT(onApplySearch())));
}

void LookupLineEditWidget::reset()
{
	lineEdit->clear();
}

QString LookupLineEditWidget::currentSearchString() const
{
	return lineEdit->text();
}

void LookupLineEditWidget::onApplySearch()
{
	emit applySearch(lineEdit->text());

	m_counter->increment();
}

void LookupLineEditWidget::onTextChanged()
{
	// Do animation for clearButton
}

void LookupLineEditWidget::setCurrentSearchData(const QString& searchData)
{
	lineEdit->setText(searchData);
}

}