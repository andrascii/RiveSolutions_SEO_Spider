#pragma once

#include "settings_page.h"
#include "ui_data_extraction_settings_widget.h"

namespace SeoSpider
{

class DataExtractionSettingsWidget : public SettingsPage
{
    Q_OBJECT

public:
    DataExtractionSettingsWidget(QWidget* parent = nullptr);

protected:
    virtual void init() override;

private slots:
    void extractorRuleTypeChanged(int index);

private:
    virtual void onShow() override;
    virtual void onClose() override;

    void configureDependentControls(QComboBox* comboBox, int newIndex);

private:
    struct DependantControls
    {
        QWidget* extractorRuleControl;
        QWidget* extractionTypeControl;

        void hide();
        void show();
    };

    Ui::DataExtractionSettingsWidget m_ui;
    std::map<QComboBox*, DependantControls> m_dependantControls;
};

}