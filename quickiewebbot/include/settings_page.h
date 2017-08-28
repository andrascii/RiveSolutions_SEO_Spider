#pragma once

#include "isettings_page.h"
#include "application_properties.h"
#include "application.h"

namespace QuickieWebBot
{

template <typename UiType>
class SettingsPage 
	: public QWidget
	, public ISettingsPage
	, protected UiType
{
public:

	SettingsPage(QWidget* parent = nullptr)
		: QWidget(parent)
	{
		setupUi(this);
		initialize();
	}

	virtual void applyChanges() noexcept override;
	virtual void reloadSettings() noexcept override;
	virtual bool isAutoApply() const noexcept override;

private:
	void initialize()
	{
		foreach(QObject* control, children())
		{
			if (!control->property("controlKey").isValid())
			{
				continue;
			}

			QString controlKey = qvariant_cast<QString>(control->property("controlKey"));
			control->setProperty(controlKey, theApp->properties()->property(controlKey));
		}
	}
};

}
