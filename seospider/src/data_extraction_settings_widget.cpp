#include "data_extraction_settings_widget.h"
#include "application.h"
#include "preferences.h"
#include "helpers.h"
#include "extractor_rule_type.h"
#include "extraction_type.h"

namespace
{

constexpr size_t c_extractorCount = 10;
constexpr int c_inactiveExtractorStateValue = -1;

}

namespace SeoSpider
{

DataExtractionSettingsWidget::DataExtractionSettingsWidget(QWidget* parent)
    : SettingsPage(parent)
{
    m_ui.setupUi(this);

    init();

    applyChanges();
}

void DataExtractionSettingsWidget::init()
{
    for (size_t i = 0; i < c_extractorCount; ++i)
    {
        const QString extractorRuleTypeObjectName = QString("extractorRuleType%1").arg(i + 1);
        const QString extractorRuleObjectName = QString("extractorRule%1").arg(i + 1);
        const QString extractionTypeObjectName = QString("extractionType%1").arg(i + 1);

        QComboBox* extractorRuleTypeComboBox = findChild<QComboBox*>(extractorRuleTypeObjectName);
        QLineEdit* extractorRuleLineEdit = findChild<QLineEdit*>(extractorRuleObjectName);
        QComboBox* extractionTypeComboBox = findChild<QComboBox*>(extractionTypeObjectName);

        ASSERT(extractorRuleTypeComboBox && "Not found some extractorRuleType object");
        ASSERT(extractorRuleLineEdit && "Not found some extractorRule object");
        ASSERT(extractionTypeComboBox && "Not found some extractionType object");

        m_dependantControls[extractorRuleTypeComboBox] =
            DependantControls{ extractorRuleLineEdit, extractionTypeComboBox };

        VERIFY(connect(extractorRuleTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DataExtractionSettingsWidget::extractorRuleTypeChanged));

        extractionTypeComboBox->addItem(tr("Extract HTML Element"),
            QVariant::fromValue(CrawlerEngine::ExtractionType::TypeExtractHtmlElement));

        extractionTypeComboBox->addItem(tr("Extract Inner HTML"),
            QVariant::fromValue(CrawlerEngine::ExtractionType::TypeExtractInnerHtml));

        extractionTypeComboBox->addItem(tr("Extract Text"),
            QVariant::fromValue(CrawlerEngine::ExtractionType::TypeExtractText));

        extractionTypeComboBox->addItem(tr("Extract Function Value"),
            QVariant::fromValue(CrawlerEngine::ExtractionType::TypeExtractFunctionValue));
    }

    for (const auto&[extractorRuleTypeControl, dependantControls] : m_dependantControls)
    {
        Q_UNUSED(dependantControls);
        extractorRuleTypeControl->addItem(tr("Inactive"), c_inactiveExtractorStateValue);
        extractorRuleTypeControl->addItem("XPath", QVariant::fromValue(CrawlerEngine::ExtractorRuleType::TypeXPath));
    }

    SettingsPage::init();

    for (const auto&[extractorRuleType, dependantControls] : m_dependantControls)
    {
        Q_UNUSED(dependantControls);
        configureDependentControls(extractorRuleType, extractorRuleType->currentIndex());
    }
}

void DataExtractionSettingsWidget::extractorRuleTypeChanged(int index)
{
    QComboBox* key = qobject_cast<QComboBox*>(sender());
    ASSERT(key);

    configureDependentControls(key, index);
}

void DataExtractionSettingsWidget::onShow()
{
}

void DataExtractionSettingsWidget::onClose()
{
}

void DataExtractionSettingsWidget::configureDependentControls(QComboBox* comboBox, int newIndex)
{
    if (!comboBox)
    {
        ERRLOG << "comboBox is nullptr";
        return;
    }

    const auto dependentControlsIter = m_dependantControls.find(comboBox);

    if (dependentControlsIter == m_dependantControls.end())
    {
        ERRLOG << "Cannot find dependent controls for" << comboBox->objectName();
        return;
    }

    DependantControls& dependentControls = dependentControlsIter->second;
    QVariant extractorRuleTypeData = comboBox->itemData(newIndex);

    if (extractorRuleTypeData == QVariant::Invalid)
    {
        ERRLOG << "Invalid extractorRuleType value: QVariant::Invalid";
        return;
    }

    if (extractorRuleTypeData.toInt() == c_inactiveExtractorStateValue)
    {
        dependentControls.hide();
    }
    else
    {
        dependentControls.show();
    }
}

void DataExtractionSettingsWidget::DependantControls::hide()
{
    if (extractorRuleControl)
    {
        extractorRuleControl->hide();
    }
    if (extractionTypeControl)
    {
        extractionTypeControl->hide();
    }
}

void DataExtractionSettingsWidget::DependantControls::show()
{
    if (extractorRuleControl)
    {
        extractorRuleControl->show();
    }
    if (extractionTypeControl)
    {
        extractionTypeControl->show();
    }
}

}