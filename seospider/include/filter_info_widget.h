#pragma once

#include "filter_info_factory.h"

namespace SeoSpider
{

class FilterInfoWidget : public QFrame
{
	Q_OBJECT

public:
	FilterInfoWidget(QWidget* parent);

	void setFilterInfo(const FilterInfo& filterInfo);

private slots:
	void onPropertyChanged();

private:
	QLabel* m_title;
	QLabel* m_description;
	FilterInfo m_filterInfo;
};

}