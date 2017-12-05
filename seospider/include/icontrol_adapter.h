#pragma once
#include "settings_page.h"

namespace SeoSpider
{

class IControlAdapter
{
public:
	virtual ~IControlAdapter() = default;
	
	virtual void setControl(QObject* control) = 0;
	virtual QVariant value() const = 0;
	virtual void setValue(const QVariant& val) = 0;
	virtual void connectChangesObserver(SettingsPage* page) = 0;

	virtual QObject* qobject() = 0;
};

}