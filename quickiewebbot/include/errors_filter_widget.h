#pragma once

#include "ui_errors_filter.h"

namespace QuickieWebBot
{

class ErrorsFilterWidget 
	: public QWidget
	, public Ui::ErrorsFilter
{
public:
	ErrorsFilterWidget(QWidget* parent = nullptr);

private:

};

}
