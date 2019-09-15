#pragma once
#include "icontrol_adapter.h"

namespace SeoSpider
{

class ControlAdapterQPlainTextEdit : public IControlAdapter
{
public:
	virtual void setControl(QObject* control) override;
	virtual QVariant value() const override;
	virtual void setValue(const QVariant& val) override;
	virtual void connectChangesObserver(SettingsPage* page) override;

	virtual QObject* qobject() override;

private:
	QPointer<QPlainTextEdit> m_control;
};

}

Q_DECLARE_METATYPE(SeoSpider::ControlAdapterQPlainTextEdit);
