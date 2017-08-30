#pragma once

#include "isettings_page.h"

namespace QuickieWebBot
{

class IControlAdaptor;

class SettingsPage 
	: public QWidget
	, public ISettingsPage
{
public:
	SettingsPage(bool isAutoApply = false, QWidget* parent = nullptr);

	virtual void applyChanges() noexcept override;
	virtual void reloadSettings() noexcept override;
	virtual bool isAutoApply() const noexcept override;

protected:
	void init();
	void registerMetaTypes();
	std::shared_ptr<IControlAdaptor> createControlAdaptor(QObject* control);

private:
	const bool m_isAutoApply;

	std::map<QString, std::shared_ptr<IControlAdaptor>> m_controlAdaptors;
};

}