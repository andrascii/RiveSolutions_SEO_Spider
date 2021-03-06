#include "stdafx.h"
#include "site_map_creator_dialog.h"
#include "application.h"
#include "main_window.h"
#include "site_map.h"
#include "crawler.h"
#include "svg_renderer.h"
#include "service_locator.h"
#include "inotification_service.h"

namespace SeoSpider
{

using namespace CrawlerEngine;

SitemapCreatorDialog::SitemapCreatorDialog(QWidget* parent)
	: Dialog(parent)
	, m_ui(new Ui_SitemapCreatorDialog)
{
	setObjectName("SitemapCreatorDialog");

	m_ui->setupUi(this);

	setWindowTitle("XML Sitemap Creator");

	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
	flags = flags & (~helpFlag);
	setWindowFlags(flags);

	setWindowIcon(SvgRenderer::render(QStringLiteral(":/images/xml-sitemap-icon.svg"), 8, 8));

	VERIFY(connect(m_ui->cancelButton, &QPushButton::clicked, this, &SitemapCreatorDialog::accept));
	VERIFY(connect(m_ui->createButton, &QPushButton::clicked, this, &SitemapCreatorDialog::showSaveFileDialog));

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

	m_ui->lastmodDateComboBox->addItem(tr("User Server Response Headers"), QVariant::fromValue(SiteMapLastModTagMode::Calculated));
	m_ui->lastmodDateComboBox->addItem(tr("Manual"), QVariant::fromValue(SiteMapLastModTagMode::Manual));
	m_ui->lastmodDateComboBox->setCurrentIndex(static_cast<int>(SiteMapLastModTagMode::Calculated));
	m_ui->customDateEdit->setDate(QDate::currentDate());

	VERIFY(connect(m_ui->lastmodDateComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(controlStateChanged())));

	controlStateChanged();
}

void SitemapCreatorDialog::showSaveFileDialog()
{
	if (theApp->crawler()->state() == Crawler::StateWorking)
	{
		Dialog::showDeferredMessageBoxDialog(
			tr("Error"),
			tr("Cannot create site map when crawler is working!"),
			QDialogButtonBox::Ok
		);

		close();
		return;
	}

	if (theApp->crawler()->hasNoData())
	{
		Dialog::showDeferredMessageBoxDialog(
			tr("What?"),
			tr("Crawler does not contain any data. It does not make sense to create empty site map. Are you agree?"),
			QDialogButtonBox::Ok
		);

		close();
		return;
	}

	const QString path = QFileDialog::getSaveFileName(theApp->mainWindow());

	INFOLOG << "Site map file path:" << path;

	SiteMapSettings sitemapSettings;

	if (m_ui->includeNoindexPagesCheckBox->isChecked())
	{
		sitemapSettings.flags.setFlag(IncludeNoIndexPages);
	}
	if (m_ui->includeCanonicolisedCheckBox->isChecked())
	{
		sitemapSettings.flags.setFlag(IncludeCanonicalised);
	}
	if (m_ui->includePaintedUrlsCheckBox->isChecked())
	{
		sitemapSettings.flags.setFlag(IncludePaginatedUrls);
	}
	if (m_ui->includePdfsCheckBox->isChecked())
	{
		sitemapSettings.flags.setFlag(IncludePDFs);
	}
	if (m_ui->includeLastmodTagCheckBox->isChecked())
	{
		sitemapSettings.flags.setFlag(IncludeLastModTag);
	}
	if (m_ui->includePriorityTagCheckBox->isChecked())
	{
		sitemapSettings.flags.setFlag(IncludePriorityTag);
	}
	if (m_ui->includeChangefreqTagCheckBox->isChecked())
	{
		sitemapSettings.flags.setFlag(IncludeChangeFreqTag);
	}
	if (m_ui->includeImagesCheckBox->isChecked())
	{
		sitemapSettings.flags.setFlag(IncludeImages);
	}
	if (m_ui->includeNoindexImagesCheckBox->isChecked())
	{
		sitemapSettings.flags.setFlag(IncludeNoIndexImages);
	}
	if (m_ui->includeOnlyRelevantImages->isChecked())
	{
		sitemapSettings.flags.setFlag(IncludeOnlyRelevantImages);
	}

	sitemapSettings.lastModifiedMode = static_cast<SiteMapLastModTagMode>(m_ui->lastmodDateComboBox->currentData().toInt());
	sitemapSettings.lastModifiedDate = m_ui->customDateEdit->dateTime();
	sitemapSettings.minimumInLinksForImages = m_ui->inlinksSpinBox->value();

	sitemapSettings.priorityLevelSettings[0] = m_ui->priorityCrawlDepth0SpinBox->value();
	sitemapSettings.priorityLevelSettings[1] = m_ui->priorityCrawlDepth1SpinBox->value();
	sitemapSettings.priorityLevelSettings[2] = m_ui->priorityCrawlDepth2SpinBox->value();
	sitemapSettings.priorityLevelSettings[3] = m_ui->priorityCrawlDepth3SpinBox->value();
	sitemapSettings.priorityLevelSettings[4] = m_ui->priorityCrawlDepth4SpinBox->value();
	sitemapSettings.priorityLevelSettings[5] = m_ui->priorityCrawlDepth5PlusSpinBox->value();

	sitemapSettings.changeFreqMode = static_cast<SiteMapChangeFreqTagMode>(m_ui->changefreqComboBox->currentData().toInt());

	sitemapSettings.changeFreqLevelSettings[0] = static_cast<SitemapChangeFreq>(m_ui->changefreqCrawlDepth0ComboBox->currentData().toInt());
	sitemapSettings.changeFreqLevelSettings[1] = static_cast<SitemapChangeFreq>(m_ui->changefreqCrawlDepth1ComboBox->currentData().toInt());
	sitemapSettings.changeFreqLevelSettings[2] = static_cast<SitemapChangeFreq>(m_ui->changefreqCrawlDepth2ComboBox->currentData().toInt());
	sitemapSettings.changeFreqLevelSettings[3] = static_cast<SitemapChangeFreq>(m_ui->changefreqCrawlDepth3ComboBox->currentData().toInt());
	sitemapSettings.changeFreqLevelSettings[4] = static_cast<SitemapChangeFreq>(m_ui->changefreqCrawlDepth4ComboBox->currentData().toInt());
	sitemapSettings.changeFreqLevelSettings[5] = static_cast<SitemapChangeFreq>(m_ui->changefreqCrawlDepth5PlusComboBox->currentData().toInt());

	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		Dialog::showDeferredMessageBoxDialog(
			tr("Error"),
			tr("Cannot create the site map file: ") + path,
			QDialogButtonBox::Ok
		);

		INFOLOG << "Cannot create the site map file:" << path;
		return;
	}

	file.write(theApp->crawler()->siteMapXml(sitemapSettings).toUtf8());

	INFOLOG << "Site map has been created:" << path;

	ServiceLocator* serviceLocator = ServiceLocator::instance();
	serviceLocator->service<INotificationService>()->info(tr("Site map"), tr("Site map successefully created!"));
	close();
}

void SitemapCreatorDialog::controlStateChanged() const
{
	const bool isIncludeLastModTagChecked = m_ui->includeLastmodTagCheckBox->isChecked();
	const bool isIncludeChangefreqTagChecked = m_ui->includeChangefreqTagCheckBox->isChecked();
	const bool isIncludeImagesChecked = m_ui->includeImagesCheckBox->isChecked();
	const bool isIncludePriorityTagChecked = m_ui->includePriorityTagCheckBox->isChecked();

	//
	// Include lastmod tag
	//
	SiteMapLastModTagMode lastmodTagMode = static_cast<SiteMapLastModTagMode>(m_ui->lastmodDateComboBox->currentData().toInt());
	m_ui->lastmodDateComboBox->setEnabled(isIncludeLastModTagChecked);
	m_ui->customDateEdit->setEnabled(isIncludeLastModTagChecked && lastmodTagMode == SiteMapLastModTagMode::Manual);

	//
	// Include changefreq tag
	//
	SiteMapChangeFreqTagMode changeFreqMode = static_cast<SiteMapChangeFreqTagMode>(m_ui->changefreqComboBox->currentData().toInt());

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