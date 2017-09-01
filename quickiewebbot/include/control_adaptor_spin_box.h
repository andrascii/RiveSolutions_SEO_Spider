#pragma once
#include "icontrol_adaptor.h"

namespace QuickieWebBot
{

	class ControlAdaptorQSpinBox : public IControlAdaptor
	{
	public:
		virtual void setControl(QObject* control) override;
		virtual QVariant value() const override;
		virtual void setValue(const QVariant& val) override;
		virtual void connectChangesObserver(SettingsPage* page) override;

	private:
		QPointer<QSpinBox> m_control;
	};

	Q_DECLARE_METATYPE(ControlAdaptorQSpinBox);

}
#pragma once
