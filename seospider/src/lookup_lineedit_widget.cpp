#include "lookup_lineedit_widget.h"
#include "svg_renderer.h"
#include "statistic_counter.h"
#include "helpers.h"
#include "cursor_factory.h"
#include "application.h"
#include "preferences.h"

namespace SeoSpider
{

LookupLineEditWidget::LookupLineEditWidget(QWidget* parent)
	: QFrame(parent)
	, m_counter(new Common::StatisticCounter(QStringLiteral("LookupLineEditWidgetApplySearchCounter"), this))
	, m_clearButtonAnimation(nullptr)
	, m_animationProcess(ProcessNone)
{
	setupUi(this);

	clearButton->hide();
	clearButton->setCursor(CursorFactory::createCursor(Qt::PointingHandCursor));
	applyButton->setMaximumWidth(clearButtonMaxWidth());
	applyButton->setMinimumWidth(clearButtonMaxWidth());
	applyButton->setCursor(CursorFactory::createCursor(Qt::PointingHandCursor));
	applyButton->setEnabled(false);

	VERIFY(connect(clearButton, &QPushButton::clicked, this, &LookupLineEditWidget::onClearButtonClicked));
	VERIFY(connect(applyButton, &QPushButton::clicked, this, &LookupLineEditWidget::onApplySearch));
	VERIFY(connect(lineEdit, &QLineEdit::returnPressed, this, &LookupLineEditWidget::onApplySearch));
	VERIFY(connect(lineEdit, SIGNAL(textChanged(const QString&)), SLOT(onTextChanged())));
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
	constexpr int animationDuration = 150;
	const bool needToHideClearButton = lineEdit->text().isEmpty();
	applyButton->setEnabled(!needToHideClearButton);

	if (m_clearButtonAnimation && needToHideClearButton)
	{
		INFOLOG << "Skip animation call";
		return;
	}

	if (m_animationProcess == ProcessHide)
	{
		m_clearButtonAnimation->stop();
		m_clearButtonAnimation->deleteLater();
		m_clearButtonAnimation = nullptr;
	}

	if (!clearButton->isVisible())
	{
		// we do this only once
		clearButton->setMaximumWidth(0);
		clearButton->setMinimumWidth(0);
		clearButton->show();
	}

	m_clearButtonAnimation = new QParallelAnimationGroup(this);

	QPropertyAnimation* maxWidthAnimation = new QPropertyAnimation(clearButton, "maximumWidth");
	QPropertyAnimation* minWidthAnimation = new QPropertyAnimation(clearButton, "minimumWidth");
	maxWidthAnimation->setDuration(animationDuration);
	minWidthAnimation->setDuration(animationDuration);

	if (needToHideClearButton)
	{
		maxWidthAnimation->setStartValue(clearButton->width());
		maxWidthAnimation->setEndValue(0);
		minWidthAnimation->setStartValue(clearButton->width());
		minWidthAnimation->setEndValue(0);

		m_animationProcess = ProcessHide;
	}
	else
	{
		if (clearButton->width() == clearButtonMaxWidth())
		{
			onAnimationFinished();
			return;
		}

		maxWidthAnimation->setStartValue(clearButton->width());
		maxWidthAnimation->setEndValue(clearButtonMaxWidth());
		minWidthAnimation->setStartValue(clearButton->width());
		minWidthAnimation->setEndValue(clearButtonMaxWidth());

		m_animationProcess = ProcessShow;
	}

	m_clearButtonAnimation->addAnimation(minWidthAnimation);
	m_clearButtonAnimation->addAnimation(maxWidthAnimation);
	m_clearButtonAnimation->start();

	VERIFY(connect(m_clearButtonAnimation, &QAbstractAnimation::finished, this, &LookupLineEditWidget::onAnimationFinished));
}

void LookupLineEditWidget::onAnimationFinished()
{
	m_clearButtonAnimation->deleteLater();
	m_clearButtonAnimation = nullptr;

	m_animationProcess = ProcessNone;
}

void LookupLineEditWidget::setCurrentSearchData(const QString& searchData)
{
	lineEdit->setText(searchData);
}

void LookupLineEditWidget::onClearButtonClicked()
{
	lineEdit->clear();
	onApplySearch();
}

int LookupLineEditWidget::clearButtonMaxWidth() const
{
	const QString locale = Preferences::localeFromString(QStringLiteral("Russian"));

	if (locale == theApp->preferences()->applicationLanguage())
	{
		return Common::Helpers::pointsToPixels(90);
	}

	return Common::Helpers::pointsToPixels(50);
}

}