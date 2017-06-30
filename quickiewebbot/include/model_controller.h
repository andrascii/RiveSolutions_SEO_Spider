#pragma once

#include "website_analyse_element.h"
#include "model_controller_data.h"

namespace QuickieWebBot
{

class ModelController 
	: public QObject
	, public ModelControllerData
{
	Q_OBJECT

public:
	ModelController(QObject* parent = nullptr);
	

};

}