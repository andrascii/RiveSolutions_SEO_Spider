#pragma once
#include "icontrol_adaptor.h"

namespace QuickieWebBot
{

class ControlAdaptorQCheckBox : public IControlAdaptor
{
public:
	virtual void setPropertyControl(QObject* control) override;
	virtual QVariant value() const override;
	virtual void setValue(const QVariant& val) override;

private:
	QPointer<QCheckBox> m_control;
};

Q_DECLARE_METATYPE(ControlAdaptorQCheckBox);

}
