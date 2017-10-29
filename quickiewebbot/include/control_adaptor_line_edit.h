#pragma once
#include "icontrol_adaptor.h"

namespace QuickieWebBot
{

class ControlAdaptorQLineEdit : public IControlAdaptor
{
public:
	virtual void setControl(QObject* control) override;
	virtual QVariant value() const override;
	virtual void setValue(const QVariant& val) override;
	virtual void connectChangesObserver(SettingsPage* page) override;

	virtual QObject* qobject() override;

private:
	QPointer<QLineEdit> m_control;
};

Q_DECLARE_METATYPE(ControlAdaptorQLineEdit);

}
