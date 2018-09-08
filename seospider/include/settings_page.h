#pragma once

#include "isettings_page.h"

namespace SeoSpider
{

class IControlAdapter;
class InternalSettingsHelper;

class SettingsPage : public QFrame, public ISettingsPage
{
	Q_OBJECT

public:
	friend class ApplicationSettingsDialog;

	SettingsPage(bool isAutoApply = false, QWidget* parent = nullptr);

	virtual void applyChanges() override;
	virtual void reloadSettings() override;
	virtual bool isAutoApply() const noexcept override;
	void setSomethingChanged(bool val) noexcept;
	void clearChangedKeys() noexcept;

	Q_SLOT void somethingChangedSlot();
	Q_SIGNAL void somethingChangedSignal();

protected:
	virtual void init();
	InternalSettingsHelper* registrateInternalHelperControl(const QVariant& initialValue = QVariant());

private:
	void registerMetaTypes() const;
	std::shared_ptr<IControlAdapter> createControlAdapter(QObject* control);
	void initControlsWithProperty();

	virtual void onShow() = 0;
	virtual void onClose() = 0;

private:
	const bool m_isAutoApply;
	bool m_somethingChanged;
	std::map<QString, std::shared_ptr<IControlAdapter>> m_controlAdapters;
	QStringList m_changedSettingsKeys;
	QList<QObject*> m_controls;
};

}