#pragma once

#include "settings_page.h"
#include "ui_yandex_metrica_settings_widget.h"

namespace SeoSpider
{

class InternalSettingsHelper;

class YandexMetricaSettingsWidget : public SettingsPage
{
	Q_OBJECT

public:
	YandexMetricaSettingsWidget(QWidget* parent = nullptr);

	virtual void applyChanges() override;

protected:
	virtual void init() override;
	virtual bool eventFilter(QObject* object, QEvent* event) override;

private slots:
	void onInternalValueChanged(const QVariant& value);
	void decrementHelperValue();
	void incrementHelperValue();
	void onSearchCounterChanged(const QVariant&);
	void validateHelperControlValue();

private:
	void validateButtonsVisibility(int currentVisibleLineEditCount);
	void validateLineEditsEnabled();

	virtual void onShow() override;
	virtual void onClose() override;

private:
	Ui::YandexMetricaSettingsWidget m_ui;
	QList<QWidget*> m_lineEdits;
	QList<InternalSettingsHelper*> m_useCounterBooleanHelpers;
	InternalSettingsHelper* m_helperControl;
	int m_visibleLineEditCount;
	int m_visibleLineEditCountAtShow;
	bool m_visibleLineEditCountApplied;
	bool m_applyingChanges;
};

}