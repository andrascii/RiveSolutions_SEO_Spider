#pragma once

#include "icontrol_adapter.h"

namespace SeoSpider
{

class InternalSettingsHelper;

class ControlAdapterInternalSettingsHelper : public IControlAdapter
{
public:
	virtual void setControl(QObject* control) override;
	virtual QVariant value() const override;
	virtual void setValue(const QVariant& val) override;
	virtual void connectChangesObserver(SettingsPage* page) override;

	virtual QObject* qobject() override;

private:
	QPointer<InternalSettingsHelper> m_control;
};

Q_DECLARE_METATYPE(ControlAdapterInternalSettingsHelper);

}