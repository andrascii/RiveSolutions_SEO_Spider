#pragma once
#include "control_binding.h"

namespace QuickieWebBot
{

class ControlBinding_CheckBox 
	: public QCheckBox
	, public ControlBinding
{
	Q_OBJECT

public:
	ControlBinding_CheckBox(QWidget* parent = nullptr);

	//virtual QVariant value() const override;
	//virtual void setValue(const QVariant& val) override;

private:
	//bool m_isUrlCorrect;
};

}
