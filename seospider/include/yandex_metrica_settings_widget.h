#pragma once

#include "settings_page.h"
#include "ui_yandex_metrica_settings_widget.h"

namespace SeoSpider
{

class InternalSettingsHelper;

class YandexMetricaSettingsWidget : public SettingsPage
{
public:
	Q_OBJECT

public:
	YandexMetricaSettingsWidget(QWidget* parent = nullptr);

protected:
	virtual void init() override;
	virtual bool eventFilter(QObject* object, QEvent* event) override;

private slots:
	void onInternalValueChanged(const QVariant& value);
	void decrementHelperValue();
	void incrementHelperValue();
	void onSearchCounterChanged(const QVariant&);

private:
	void validateButtonsVisibility(int currentVisibleLineEditCount);

private:
	Ui::YandexMetricaSettingsWidget m_ui;
	QList<QWidget*> m_lineEdits;
	QList<InternalSettingsHelper*> m_useCounterBooleanHelpers;
	InternalSettingsHelper* m_helperControl;
	int m_visibleLineEditCount;
};

}