#include "site_map_creator_widget.h"
#include "application.h"
#include "main_window.h"
#include "site_map.h"

namespace
{

using namespace SeoSpider;

// const std::map<std::string_view, SiteMapChangeFreqTagMode> s_stringValue = 
// {
// 	{ "Calculate from Last Modified Header", SiteMapChangeFreqTagMode::CalculatedFromLastModifiedHeader },
// 	{ "Use Crawl Depth Settings", SiteMapChangeFreqTagMode::UseLevelSettings }
// };

}

namespace SeoSpider
{

using namespace CrawlerEngine;

SitemapCreatorWidget::SitemapCreatorWidget(QWidget* parent)
	: QDialog(parent)
	, m_ui(new Ui_SitemapCreatorWidget)
{
	m_ui->setupUi(this);

	setWindowTitle("XML Sitemap Creator");

	VERIFY(connect(m_ui->cancelButton, &QPushButton::clicked, this, &SitemapCreatorWidget::accept));
	VERIFY(connect(m_ui->createButton, &QPushButton::clicked, this, &SitemapCreatorWidget::showSaveFileDialog));

	VERIFY(connect(m_ui->includeLastmodTagCheckBox, SIGNAL(stateChanged(int)), this, SLOT(controlStateChanged())));
	VERIFY(connect(m_ui->includeChangefreqTagCheckBox, SIGNAL(stateChanged(int)), this, SLOT(controlStateChanged())));
	VERIFY(connect(m_ui->includeImagesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(controlStateChanged())));
	VERIFY(connect(m_ui->includePriorityTagCheckBox, SIGNAL(stateChanged(int)), this, SLOT(controlStateChanged())));

	m_ui->changefreqComboBox->addItem(tr("Calculate from Last Modified Header"), QVariant::fromValue(SiteMapChangeFreqTagMode::CalculatedFromLastModifiedHeader));
	m_ui->changefreqComboBox->addItem(tr("Use Crawl Depth Settings"), QVariant::fromValue(SiteMapChangeFreqTagMode::UseLevelSettings));

	VERIFY(connect(m_ui->changefreqComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(controlStateChanged())));

	m_ui->changefreqCrawlDepth0ComboBox->addItem(tr("Always"), QVariant::fromValue(SitemapChangeFreq::Always));
	m_ui->changefreqCrawlDepth0ComboBox->addItem(tr("Hourly"), QVariant::fromValue(SitemapChangeFreq::Hourly));
	m_ui->changefreqCrawlDepth0ComboBox->addItem(tr("Daily"), QVariant::fromValue(SitemapChangeFreq::Daily));
	m_ui->changefreqCrawlDepth0ComboBox->addItem(tr("Weekly"), QVariant::fromValue(SitemapChangeFreq::Weekly));
	m_ui->changefreqCrawlDepth0ComboBox->addItem(tr("Monthly"), QVariant::fromValue(SitemapChangeFreq::Monthly));
	m_ui->changefreqCrawlDepth0ComboBox->addItem(tr("Yearly"), QVariant::fromValue(SitemapChangeFreq::Yearly));
	m_ui->changefreqCrawlDepth0ComboBox->addItem(tr("Never"), QVariant::fromValue(SitemapChangeFreq::Never));
	m_ui->changefreqCrawlDepth0ComboBox->setCurrentIndex(static_cast<int>(SitemapChangeFreq::Daily));

	m_ui->changefreqCrawlDepth1ComboBox->addItem(tr("Always"), QVariant::fromValue(SitemapChangeFreq::Always));
	m_ui->changefreqCrawlDepth1ComboBox->addItem(tr("Hourly"), QVariant::fromValue(SitemapChangeFreq::Hourly));
	m_ui->changefreqCrawlDepth1ComboBox->addItem(tr("Daily"), QVariant::fromValue(SitemapChangeFreq::Daily));
	m_ui->changefreqCrawlDepth1ComboBox->addItem(tr("Weekly"), QVariant::fromValue(SitemapChangeFreq::Weekly));
	m_ui->changefreqCrawlDepth1ComboBox->addItem(tr("Monthly"), QVariant::fromValue(SitemapChangeFreq::Monthly));
	m_ui->changefreqCrawlDepth1ComboBox->addItem(tr("Yearly"), QVariant::fromValue(SitemapChangeFreq::Yearly));
	m_ui->changefreqCrawlDepth1ComboBox->addItem(tr("Never"), QVariant::fromValue(SitemapChangeFreq::Never));
	m_ui->changefreqCrawlDepth1ComboBox->setCurrentIndex(static_cast<int>(SitemapChangeFreq::Daily));

	m_ui->changefreqCrawlDepth2ComboBox->addItem(tr("Always"), QVariant::fromValue(SitemapChangeFreq::Always));
	m_ui->changefreqCrawlDepth2ComboBox->addItem(tr("Hourly"), QVariant::fromValue(SitemapChangeFreq::Hourly));
	m_ui->changefreqCrawlDepth2ComboBox->addItem(tr("Daily"), QVariant::fromValue(SitemapChangeFreq::Daily));
	m_ui->changefreqCrawlDepth2ComboBox->addItem(tr("Weekly"), QVariant::fromValue(SitemapChangeFreq::Weekly));
	m_ui->changefreqCrawlDepth2ComboBox->addItem(tr("Monthly"), QVariant::fromValue(SitemapChangeFreq::Monthly));
	m_ui->changefreqCrawlDepth2ComboBox->addItem(tr("Yearly"), QVariant::fromValue(SitemapChangeFreq::Yearly));
	m_ui->changefreqCrawlDepth2ComboBox->addItem(tr("Never"), QVariant::fromValue(SitemapChangeFreq::Never));
	m_ui->changefreqCrawlDepth2ComboBox->setCurrentIndex(static_cast<int>(SitemapChangeFreq::Daily));

	m_ui->changefreqCrawlDepth3ComboBox->addItem(tr("Always"), QVariant::fromValue(SitemapChangeFreq::Always));
	m_ui->changefreqCrawlDepth3ComboBox->addItem(tr("Hourly"), QVariant::fromValue(SitemapChangeFreq::Hourly));
	m_ui->changefreqCrawlDepth3ComboBox->addItem(tr("Daily"), QVariant::fromValue(SitemapChangeFreq::Daily));
	m_ui->changefreqCrawlDepth3ComboBox->addItem(tr("Weekly"), QVariant::fromValue(SitemapChangeFreq::Weekly));
	m_ui->changefreqCrawlDepth3ComboBox->addItem(tr("Monthly"), QVariant::fromValue(SitemapChangeFreq::Monthly));
	m_ui->changefreqCrawlDepth3ComboBox->addItem(tr("Yearly"), QVariant::fromValue(SitemapChangeFreq::Yearly));
	m_ui->changefreqCrawlDepth3ComboBox->addItem(tr("Never"), QVariant::fromValue(SitemapChangeFreq::Never));
	m_ui->changefreqCrawlDepth3ComboBox->setCurrentIndex(static_cast<int>(SitemapChangeFreq::Daily));

	m_ui->changefreqCrawlDepth4ComboBox->addItem(tr("Always"), QVariant::fromValue(SitemapChangeFreq::Always));
	m_ui->changefreqCrawlDepth4ComboBox->addItem(tr("Hourly"), QVariant::fromValue(SitemapChangeFreq::Hourly));
	m_ui->changefreqCrawlDepth4ComboBox->addItem(tr("Daily"), QVariant::fromValue(SitemapChangeFreq::Daily));
	m_ui->changefreqCrawlDepth4ComboBox->addItem(tr("Weekly"), QVariant::fromValue(SitemapChangeFreq::Weekly));
	m_ui->changefreqCrawlDepth4ComboBox->addItem(tr("Monthly"), QVariant::fromValue(SitemapChangeFreq::Monthly));
	m_ui->changefreqCrawlDepth4ComboBox->addItem(tr("Yearly"), QVariant::fromValue(SitemapChangeFreq::Yearly));
	m_ui->changefreqCrawlDepth4ComboBox->addItem(tr("Never"), QVariant::fromValue(SitemapChangeFreq::Never));
	m_ui->changefreqCrawlDepth4ComboBox->setCurrentIndex(static_cast<int>(SitemapChangeFreq::Daily));

	m_ui->changefreqCrawlDepth5PlusComboBox->addItem(tr("Always"), QVariant::fromValue(SitemapChangeFreq::Always));
	m_ui->changefreqCrawlDepth5PlusComboBox->addItem(tr("Hourly"), QVariant::fromValue(SitemapChangeFreq::Hourly));
	m_ui->changefreqCrawlDepth5PlusComboBox->addItem(tr("Daily"), QVariant::fromValue(SitemapChangeFreq::Daily));
	m_ui->changefreqCrawlDepth5PlusComboBox->addItem(tr("Weekly"), QVariant::fromValue(SitemapChangeFreq::Weekly));
	m_ui->changefreqCrawlDepth5PlusComboBox->addItem(tr("Monthly"), QVariant::fromValue(SitemapChangeFreq::Monthly));
	m_ui->changefreqCrawlDepth5PlusComboBox->addItem(tr("Yearly"), QVariant::fromValue(SitemapChangeFreq::Yearly));
	m_ui->changefreqCrawlDepth5PlusComboBox->addItem(tr("Never"), QVariant::fromValue(SitemapChangeFreq::Never));
	m_ui->changefreqCrawlDepth5PlusComboBox->setCurrentIndex(static_cast<int>(SitemapChangeFreq::Daily));

	controlStateChanged();
}

void SitemapCreatorWidget::showSaveFileDialog() const
{
	QFileDialog::getSaveFileName(theApp->mainWindow());
}

void SitemapCreatorWidget::controlStateChanged() const
{
	const bool isIncludeLastModTagChecked = m_ui->includeLastmodTagCheckBox->isChecked();
	const bool isIncludeChangefreqTagChecked = m_ui->includeChangefreqTagCheckBox->isChecked();
	const bool isIncludeImagesChecked = m_ui->includeImagesCheckBox->isChecked();
	const bool isIncludePriorityTagChecked = m_ui->includePriorityTagCheckBox->isChecked();

	//
	// Include lastmod tag
	//
	m_ui->lastmodDateComboBox->setEnabled(isIncludeLastModTagChecked);

	//
	// Include changefreq tag
	//
	SiteMapChangeFreqTagMode changeFreqMode = static_cast<SiteMapChangeFreqTagMode>(m_ui->changefreqComboBox->currentData(Qt::UserRole).toInt());

	m_ui->changefreqComboBox->setEnabled(isIncludeChangefreqTagChecked);
	m_ui->changefreqCrawlDepth0ComboBox->setEnabled(isIncludeChangefreqTagChecked && changeFreqMode == SiteMapChangeFreqTagMode::UseLevelSettings);
	m_ui->changefreqCrawlDepth1ComboBox->setEnabled(isIncludeChangefreqTagChecked && changeFreqMode == SiteMapChangeFreqTagMode::UseLevelSettings);
	m_ui->changefreqCrawlDepth2ComboBox->setEnabled(isIncludeChangefreqTagChecked && changeFreqMode == SiteMapChangeFreqTagMode::UseLevelSettings);
	m_ui->changefreqCrawlDepth3ComboBox->setEnabled(isIncludeChangefreqTagChecked && changeFreqMode == SiteMapChangeFreqTagMode::UseLevelSettings);
	m_ui->changefreqCrawlDepth4ComboBox->setEnabled(isIncludeChangefreqTagChecked && changeFreqMode == SiteMapChangeFreqTagMode::UseLevelSettings);
	m_ui->changefreqCrawlDepth5PlusComboBox->setEnabled(isIncludeChangefreqTagChecked && changeFreqMode == SiteMapChangeFreqTagMode::UseLevelSettings);
	m_ui->label_14->setEnabled(isIncludeChangefreqTagChecked && changeFreqMode == SiteMapChangeFreqTagMode::UseLevelSettings);
	m_ui->label_15->setEnabled(isIncludeChangefreqTagChecked && changeFreqMode == SiteMapChangeFreqTagMode::UseLevelSettings);
	m_ui->label_16->setEnabled(isIncludeChangefreqTagChecked && changeFreqMode == SiteMapChangeFreqTagMode::UseLevelSettings);
	m_ui->label_17->setEnabled(isIncludeChangefreqTagChecked && changeFreqMode == SiteMapChangeFreqTagMode::UseLevelSettings);
	m_ui->label_18->setEnabled(isIncludeChangefreqTagChecked && changeFreqMode == SiteMapChangeFreqTagMode::UseLevelSettings);
	m_ui->label_19->setEnabled(isIncludeChangefreqTagChecked && changeFreqMode == SiteMapChangeFreqTagMode::UseLevelSettings);

	//
	// Include Images
	//
	m_ui->includeNoindexImagesCheckBox->setEnabled(isIncludeImagesChecked);
	m_ui->includeOnlyRelevantImages->setEnabled(isIncludeImagesChecked);
	m_ui->inlinksSpinBox->setEnabled(isIncludeImagesChecked);
	m_ui->label_20->setEnabled(isIncludeImagesChecked);

	//
	// Include priority tag
	//
	m_ui->priorityCrawlDepth0SpinBox->setEnabled(isIncludePriorityTagChecked);
	m_ui->priorityCrawlDepth1SpinBox->setEnabled(isIncludePriorityTagChecked);
	m_ui->priorityCrawlDepth2SpinBox->setEnabled(isIncludePriorityTagChecked);
	m_ui->priorityCrawlDepth3SpinBox->setEnabled(isIncludePriorityTagChecked);
	m_ui->priorityCrawlDepth4SpinBox->setEnabled(isIncludePriorityTagChecked);
	m_ui->priorityCrawlDepth5PlusSpinBox->setEnabled(isIncludePriorityTagChecked);
	m_ui->label_8->setEnabled(isIncludePriorityTagChecked);
	m_ui->label_9->setEnabled(isIncludePriorityTagChecked);
	m_ui->label_10->setEnabled(isIncludePriorityTagChecked);
	m_ui->label_11->setEnabled(isIncludePriorityTagChecked);
	m_ui->label_12->setEnabled(isIncludePriorityTagChecked);
	m_ui->label_13->setEnabled(isIncludePriorityTagChecked);


}

}