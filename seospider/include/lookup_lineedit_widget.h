#pragma once

#include "ui_lookup_lineedit_widget.h"

namespace SeoSpider
{

class IStatisticCounter;

class LookupLineEditWidget : public QFrame, protected Ui::LookupLineEditWidget
{
	Q_OBJECT

public:
	LookupLineEditWidget(QWidget* parent = nullptr);

	void reset();
	QString currentSearchString() const;
	void setCurrentSearchData(const QString& searchData);

signals:
	void applySearch(const QString& searchString);

private slots:
	void onApplySearch();
	void onTextChanged();
	void onAnimationFinished();
	void onClearButtonClicked();

private:
	enum AnimationProcess
	{
		ProcessNone,
		ProcessHide,
		ProcessShow
	};

	IStatisticCounter* m_counter;
	QParallelAnimationGroup* m_clearButtonAnimation;
	AnimationProcess m_animationProcess;
};

}