#pragma once

#include "ui_page_visual_settings_widget.h"
#include "settings_page.h"
#include "color_selector.h"

namespace SeoSpider
{

class PageVisualSettingsWidget : public SettingsPage
{
	Q_OBJECT
public:
	PageVisualSettingsWidget(QWidget* parent = nullptr);

protected:
	virtual bool eventFilter(QObject* object, QEvent* event) override;

private:
	virtual void onShow() override;
	virtual void onClose() override;
	virtual void init() override;

private:
	Ui::PageVisualSettingsWidget m_ui;
	QColor m_notIndexedPagesColor;
};

}