#pragma once

#include "isettings_page.h"
namespace SeoSpider
{

class IControlAdapter;

class SettingsPage : public QFrame, public ISettingsPage
{
	Q_OBJECT

public:
	SettingsPage(bool isAutoApply = false, QWidget* parent = nullptr);

	virtual void applyChanges() noexcept override;

	virtual void reloadSettings() noexcept override;

	virtual bool isAutoApply() const noexcept override;

	void setSomethingChanged(bool val) noexcept;

	void clearChangedKeys() noexcept;

	Q_SLOT void somethingChangedSlot();
	
	Q_SIGNAL void somethingChangedSignal();

protected:
	virtual void init();
	
	void registerMetaTypes() const;
	
	std::shared_ptr<IControlAdapter> createControlAdapter(QObject* control);

	void initControlsWithProperty();

private:
	const bool m_isAutoApply;
	
	bool m_somethingChanged;

	std::map<QString, std::shared_ptr<IControlAdapter>> m_controlAdapters;
	
	QStringList m_changedSettingsKeys;

	QList<QObject*> m_controls;
};

}