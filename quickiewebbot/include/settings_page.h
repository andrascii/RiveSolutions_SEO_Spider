#pragma once

#include "isettings_page.h"

namespace QuickieWebBot
{

class IControlAdaptor;

class SettingsPage 
	: public QWidget
	, public ISettingsPage
{
	Q_OBJECT

public:
	SettingsPage(bool isAutoApply = false, QWidget* parent = nullptr);

	virtual void applyChanges() noexcept override;
	virtual void reloadSettings() noexcept override;
	virtual bool isAutoApply() const noexcept override;

	void setSomethingChanged(bool val) noexcept;

	Q_SLOT void somethingChangedSlot();
	Q_SIGNAL void somethingChangedSignal();

protected:
	void init();
	void registerMetaTypes();
	std::shared_ptr<IControlAdaptor> createControlAdaptor(QObject* control);

private:
	const bool m_isAutoApply;
	bool m_somethingChanged;

	std::map<QString, std::shared_ptr<IControlAdaptor>> m_controlAdaptors;
	QStringList m_changedSettingsKeys;
};

}