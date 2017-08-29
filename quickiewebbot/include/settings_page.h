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
		initialize();
	}

	//virtual void applyChanges() noexcept override;
	//virtual void reloadSettings() noexcept override;
	//virtual bool isAutoApply() const noexcept override;

private:
	void initialize()
	{
		UiType::setupUi(this);

		QList<QWidget*> widgets = findChildren<QWidget*>();
		widgets.size();
		foreach(QWidget* control, widgets)
		{
			if (!control->property("controlKey").isValid())
			{
				continue;
			}

			const QString controlKeyString = qvariant_cast<QString>(control->property("controlKey"));

			{
				assert(control->property("controlKey").toByteArray() == controlKeyString.toLatin1());
				assert(controlKeyString.toUtf8() == controlKeyString.toLatin1());
			}

			const QByteArray controlKey = controlKeyString.toLatin1();

			if (controlKey.isEmpty())
			{
				continue;
			}

			//
			//дописать код
			//
		}
	}
};

}
