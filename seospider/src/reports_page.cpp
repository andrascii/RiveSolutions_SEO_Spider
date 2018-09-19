#include "reports_page.h"
#include "application.h"
#include "preferences.h"
#include "crawler.h"
#include "helpers.h"
#include "inotification_service.h"
#include "service_locator.h"
#include "pdf_report_exporter.h"
#include "svg_renderer.h"
#include "report_elements.h"

namespace SeoSpider
{

using namespace CrawlerEngine;

ReportsPage::ReportsPage(QWidget* parent)
	: QFrame(parent)
	, m_stackedWidget(new QStackedWidget(this))
	, m_placeHolderLabel(new QLabel(this))
	, m_reportDataProvider(theApp->crawler()->sequencedDataCollection())
	, m_updateTimerId(0)
	, m_scrollArea(new QScrollArea(this))
{
	theApp->installEventFilter(this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_stackedWidget);
	layout->setMargin(0);

	m_placeHolderLabel->setObjectName(QStringLiteral("TablePlaseholderLabel"));
	m_placeHolderLabel->setText(tr("Waiting while crawling ended"));
	m_placeHolderLabel->setAlignment(Qt::AlignCenter);
	m_stackedWidget->addWidget(m_placeHolderLabel);
	m_viewFrame = new QFrame(this);
	m_viewFrame->setObjectName("reportView");
	m_scrollArea->setWidget(m_viewFrame);
	m_stackedWidget->addWidget(m_scrollArea);
	m_scrollArea->setObjectName("reportScrollArea");
	m_scrollArea->setWidgetResizable(true);
	m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	QGridLayout* frameLayout = new QGridLayout(m_viewFrame);

	frameLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 0);
	frameLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 2);

	m_viewFrame->setLayout(frameLayout);

	m_viewFrameBody = new QFrame(this);
	m_viewFrameBody->setMinimumSize(QSize(200, 200));
	m_viewFrameBody->installEventFilter(this);

	m_viewFrameBody->setObjectName("reportViewBody");
	m_viewFrameBody->setMinimumWidth(800);
	// TODO: add resize event handler and rebuild layout if widget will be resizable

	frameLayout->addWidget(m_viewFrameBody, 0, 1);

	setReportType(ReportTypeBrief);

	VERIFY(connect(theApp->preferences(), &Preferences::companyNameChanged, this, &ReportsPage::updateContent));
	VERIFY(connect(theApp->crawler(), SIGNAL(stateChanged(int)), this, SLOT(crawlerStateChangedSlot(int))));
	VERIFY(connect(theApp->crawler(), &Crawler::onAboutClearData, this, &ReportsPage::crawlerDataCleared));

	updateLayout(m_reportType, true);
	m_stackedWidget->setCurrentWidget(m_placeHolderLabel);
}

ReportsPage::~ReportsPage()
{
	qDeleteAll(m_reportElements.values());
}

void ReportsPage::updateLayout(ReportType report, const bool rebuildLayout)
{
	QSet<QByteArray> keys = m_reportElements.keys().toSet();
	if (rebuildLayout)
	{
		foreach(const QByteArray& key, keys)
		{
			IReportElement* element = m_reportElements.value(key);
			element->widget()->hide();
			element->widget()->deleteLater();
			delete element;
		}

		m_reportElements.clear();

		buildLayout(report);
	}


	int bottom = 0;
	keys = m_reportElements.keys().toSet();

	foreach(const QByteArray& key, keys)
	{
		IReportElement* element = m_reportElements.value(key);
		element->invalidateRightBottom();
	}

	foreach(ReportDataKeys key, m_reportDataProvider.allKeys())
	{
		IReportElement* element = m_reportElements.value(m_reportDataProvider.placeholder(key), nullptr);
		if (element != nullptr)
		{
			element->setValue(m_reportDataProvider.data(key));
			element->widget()->show();
			element->moveToPosition();
			bottom = qMax(bottom, element->bottom());

			keys.remove(m_reportDataProvider.placeholder(key));
		}
	}

	foreach(const QByteArray& key, keys)
	{
		IReportElement* element = m_reportElements.value(key);
		element->widget()->show();
		element->moveToPosition();
		bottom = qMax(bottom, element->bottom());
	}

	m_viewFrame->setFixedHeight(bottom + Common::Helpers::pointsToPixels(10));
}

void ReportsPage::setReportType(ReportType reportType)
{
	const bool rebuildLayout = m_reportType != reportType;
	m_reportType = reportType;
	updateLayout(reportType, rebuildLayout);
}

void ReportsPage::timerEvent(QTimerEvent* event)
{
	Q_UNUSED(event);

	setReportType(m_reportType);
}

void ReportsPage::showEvent(QShowEvent* event)
{
	setReportType(m_reportType);

	if (theApp->crawler()->state() == CrawlerEngine::Crawler::StateWorking)
	{
		m_updateTimerId = startTimer(1000);
		DEBUG_ASSERT(m_updateTimerId);
	}

	QFrame::showEvent(event);
}

void ReportsPage::hideEvent(QHideEvent* event)
{
	killTimer(m_updateTimerId);
	m_updateTimerId = 0;

	QFrame::hideEvent(event);
}

QByteArray ReportsPage::reportMaketContent(ReportType reportType) const
{
	QFile reportMaket;
	QFile maketStyle(QStringLiteral(":/reportmaket/maket_style.css"));

	switch (reportType)
	{
		case ReportTypeBrief:
		{
			reportMaket.setFileName(QStringLiteral(":/reportmaket/brief_report_maket.html"));
			break;
		}
		case ReportTypeDetailed:
		{
			reportMaket.setFileName(QStringLiteral(":/reportmaket/detailed_report_maket.html"));
			break;
		}
	}

	const bool isMaketOpened = reportMaket.open(QIODevice::ReadOnly | QIODevice::Text);
	const bool isMaketStyleOpened = maketStyle.open(QIODevice::ReadOnly | QIODevice::Text);

	DEBUG_ASSERT(isMaketOpened);
	DEBUG_ASSERT(isMaketStyleOpened);

	if (!isMaketOpened)
	{
		ERRLOG << "Something was wrong and application not able to load report maket!";
	}

	if (!isMaketStyleOpened)
	{
		ERRLOG << "Something was wrong and application not able to load report maket style!";
	}

	QByteArray maketContent = reportMaket.readAll();
	changePlaceholderInContent("stylesheet", maketStyle.readAll(), maketContent);

	foreach(ReportDataKeys key, m_reportDataProvider.allKeys())
	{
		changePlaceholderInContent(m_reportDataProvider.placeholder(key), m_reportDataProvider.data(key), maketContent);
	}

	return maketContent;
}

void ReportsPage::changePlaceholderInContent(const QByteArray& placeholder, const QVariant& value, QByteArray& content) const
{
	if (!value.isValid())
	{
		ERRLOG << placeholder << "provided invalid data";
		return;
	}

	switch (value.type())
	{
		case QVariant::String:
		case QVariant::ByteArray:
		{
			content.replace("((" + placeholder + "))", value.toString().toUtf8());
			break;
		}
		case QVariant::Pixmap:
		{
			QByteArray pixmapData;
			QBuffer buffer(&pixmapData);

			const QPixmap& pixmap = qvariant_cast<QPixmap>(value);
			pixmap.save(&buffer, "PNG");

			const QByteArray imgTag("<img src=\"data:image/png;base64," + pixmapData.toBase64() + "\"/>");

			content.replace("((" + placeholder + "))", imgTag);

			break;
		}
		case QVariant::Int:
		case QVariant::UInt:
		case QVariant::LongLong:
		case QVariant::ULongLong:
		{
			content.replace("((" + placeholder + "))", QByteArray::number(value.toULongLong()));

			break;
		}
		default:
		{
			DEBUG_ASSERT(!"Unexpected QVariant type");
		}
	}
}

void ReportsPage::updateContent()
{
	setReportType(m_reportType);
}

void ReportsPage::exportToPdf()
{
	PdfReportExporter exporter;
	doExport(&exporter);
}

void ReportsPage::crawlerStateChangedSlot(int state)
{
	if(state != Crawler::State::StateWorking)
	{
		m_stackedWidget->setCurrentWidget(m_scrollArea);
		updateContent();
		return;
	}

	m_stackedWidget->setCurrentWidget(m_placeHolderLabel);
}

void ReportsPage::crawlerDataCleared()
{
	m_stackedWidget->setCurrentWidget(m_placeHolderLabel);
}

void ReportsPage::doExport(IReportExporter* exporter) const
{
	QString fileName = QFileDialog::getSaveFileName(0, exporter->description(), ".", exporter->fileMask());

	if (fileName.isEmpty())
	{
		return;
	}

	if (!fileName.endsWith(exporter->ext()))
	{
		fileName = fileName + exporter->ext();
	}

	QFile file(fileName);

	ServiceLocator* serviceLocator = ServiceLocator::instance();
	if (!file.open(QIODevice::WriteOnly))
	{
		serviceLocator->service<INotificationService>()->error(tr("Export"), tr("Export failed"));
	}

	exporter->doExport(&file, m_reportType, &m_reportDataProvider);
	serviceLocator->service<INotificationService>()->info(tr("Export"), tr("Export has been finished"));

	file.close();
}

void SeoSpider::ReportsPage::buildLayout(ReportType reportType)
{
	if (reportType == ReportTypeBrief)
	{
		buildLayoutBrief();
		return;
	}

	ASSERT(!"Not implemented");
}

void ReportsPage::buildLayoutBrief()
{
	using Key = ReportDataKeys;

	const int bodyMargin = Common::Helpers::pointsToPixels(10);

	const auto addSpacerItem = [this, bodyMargin](const QByteArray& spacerKey, ReportElementBase* prevElement)
	{
		const int height = Common::Helpers::pointsToPixels(15);
		ReportElementBase* spacerElement = m_reportElements.value(spacerKey, nullptr);
		if (spacerElement == nullptr)
		{
			spacerElement = new ImageReportElement("spacerElement", m_viewFrameBody);
			m_reportElements[spacerKey] = spacerElement;
		}
		spacerElement->setSize(percentWidthPixels(100), height);
		spacerElement->setRelativePosition(0, bodyMargin, prevElement, nullptr);
	};

	const auto addSummaryItem = [this](Key image, Key text, Key counter, int leftMargin, int topMargin, ReportElementBase* prevLineWidget)
	{
		const int firstLineHeight = Common::Helpers::pointsToPixels(15);
		const int secondLineHeight = Common::Helpers::pointsToPixels(22);


		ReportElementBase* imageElement = m_reportElements.value(m_reportDataProvider.placeholder(image), nullptr);
		if (imageElement == nullptr)
		{
			imageElement = new ImageReportElement("summaryElementImage", m_viewFrameBody);
			m_reportElements[m_reportDataProvider.placeholder(image)] = imageElement;
		}
		imageElement->setSize(firstLineHeight, firstLineHeight);
		imageElement->setRelativePosition(leftMargin, topMargin, prevLineWidget, nullptr);


		const QByteArray textKey = text == Key::Date ? QByteArray("symmaryDateText") : m_reportDataProvider.placeholder(text);
		ReportElementBase* textElement = m_reportElements.value(textKey, nullptr);
		if (textElement == nullptr)
		{
			textElement = new LabelReportElement("summaryElementText", m_viewFrameBody);
			if (text == Key::Date)
			{
				textElement->setValue(QObject::tr("Report Date"));
			}
			m_reportElements[textKey] = textElement;
		}
		textElement->setSize(percentWidthPixels(15), firstLineHeight);
		textElement->setRelativePosition(percentWidthPixels(1), topMargin, prevLineWidget, imageElement);

		ReportElementBase* counterElement = m_reportElements.value(m_reportDataProvider.placeholder(counter), nullptr);
		if (counterElement == nullptr)
		{
			counterElement = new LabelReportElement("summaryElementCounter", m_viewFrameBody, Qt::AlignLeft | Qt::AlignTop);
			m_reportElements[m_reportDataProvider.placeholder(counter)] = counterElement;
		}
		counterElement->setSize(percentWidthPixels(18), secondLineHeight);
		counterElement->setRelativePosition(percentWidthPixels(1), Common::Helpers::pointsToPixels(2), textElement, imageElement);
	};

	const auto addReportItem = [this, bodyMargin](Key image, Key text, Key counter, ReportElementBase* prevLineWidget, ReportElementBase* leftWidget, int extraLeftMarginPercents = 0)
	{
		const int height = Common::Helpers::pointsToPixels(15);
		const int leftMargin = extraLeftMarginPercents == 0 ? bodyMargin : percentWidthPixels(extraLeftMarginPercents);
		const int topMargin = Common::Helpers::pointsToPixels(5);

		ReportElementBase* imageElement = m_reportElements.value(m_reportDataProvider.placeholder(image), nullptr);
		if (imageElement == nullptr)
		{
			imageElement = new ImageReportElement("reportElementImage", m_viewFrameBody);
			m_reportElements[m_reportDataProvider.placeholder(image)] = imageElement;
		}
		imageElement->setSize(height, height);
		imageElement->setRelativePosition(leftMargin, topMargin, prevLineWidget, leftWidget);

		ReportElementBase* textElement = m_reportElements.value(m_reportDataProvider.placeholder(text), nullptr);
		if (textElement == nullptr)
		{
			textElement = new LabelReportElement("reportElementText", m_viewFrameBody);
			m_reportElements[m_reportDataProvider.placeholder(text)] = textElement;
		}
		textElement->setSize(percentWidthPixels(35), height);
		textElement->setRelativePosition(percentWidthPixels(1), topMargin, prevLineWidget, imageElement);

		ReportElementBase* counterElement = m_reportElements.value(m_reportDataProvider.placeholder(counter), nullptr);
		if (counterElement == nullptr)
		{
			counterElement = new LabelReportElement("reportElementCounter", m_viewFrameBody, Qt::AlignRight | Qt::AlignVCenter);
			m_reportElements[m_reportDataProvider.placeholder(counter)] = counterElement;
		}
		counterElement->setSize(percentWidthPixels(6), height);
		counterElement->setRelativePosition(percentWidthPixels(1), topMargin, prevLineWidget, textElement);
	};

	ReportElementBase* siteImage = getWidget(Key::SiteShortImage);
	if (siteImage == nullptr)
	{
		siteImage = new ImageReportElement(m_reportDataProvider.placeholder(Key::SiteShortImage), m_viewFrameBody, true);
		m_reportElements[m_reportDataProvider.placeholder(Key::SiteShortImage)] = siteImage;
	}
	siteImage->setSize(percentWidthPixels(35), percentWidthPixels(35) * 0.8);
	siteImage->setRelativePosition(bodyMargin, bodyMargin, nullptr, nullptr);


	ReportElementBase* siteLink = getWidget(Key::SiteLink);
	if (siteLink == nullptr)
	{
		siteLink = new LabelReportElement(m_reportDataProvider.placeholder(Key::SiteLink), m_viewFrameBody);
		m_reportElements[m_reportDataProvider.placeholder(Key::SiteLink)] = siteLink;
	}
	siteLink->setSize(percentWidthPixels(60), Common::Helpers::pointsToPixels(24));
	siteLink->setRelativePosition(percentWidthPixels(3), bodyMargin + Common::Helpers::pointsToPixels(5), nullptr, siteImage);

	ReportElementBase* problemsCount = getWidget(Key::FoundProblemsCount);
	if (problemsCount == nullptr)
	{
		problemsCount = new LabelReportElement(m_reportDataProvider.placeholder(Key::FoundProblemsCount), m_viewFrameBody, Qt::AlignLeft | Qt::AlignBottom);
		m_reportElements[m_reportDataProvider.placeholder(Key::FoundProblemsCount)] = problemsCount;
	}
	problemsCount->setSize(-1, Common::Helpers::pointsToPixels(28));
	problemsCount->setRelativePosition(percentWidthPixels(3), Common::Helpers::pointsToPixels(10), siteLink, siteImage);

	ReportElementBase* problemsFound = getWidget("problemsFound");
	if (problemsFound == nullptr)
	{
		problemsFound = new LabelReportElement("problemsFound", m_viewFrameBody, Qt::AlignLeft | Qt::AlignBottom);
		problemsFound->setValue(QObject::tr("problems found"));
		m_reportElements["problemsFound"] = problemsFound;
	}
	problemsFound->setSize(percentWidthPixels(35), problemsCount->height());
	problemsFound->setRelativePosition(Common::Helpers::pointsToPixels(5), Common::Helpers::pointsToPixels(6), siteLink, problemsCount);

	addSpacerItem("spacer1", siteImage);

	addSummaryItem(Key::ErrorsImage, Key::Errors, Key::ErrorsCount, bodyMargin, Common::Helpers::pointsToPixels(10), getWidget("spacer1"));
	addSummaryItem(Key::WarningsImage, Key::Warnings, Key::WarningsCount, bodyMargin + percentWidthPixels(24), Common::Helpers::pointsToPixels(10), getWidget("spacer1"));
	addSummaryItem(Key::InfoImage, Key::Info, Key::InfoCount, bodyMargin + percentWidthPixels(49), Common::Helpers::pointsToPixels(10), getWidget("spacer1"));
	addSummaryItem(Key::DateImage, Key::Date, Key::Date, bodyMargin + percentWidthPixels(74), Common::Helpers::pointsToPixels(10), getWidget("spacer1"));

	addSpacerItem("spacer2", getWidget(Key::ErrorsCount));

	ReportElementBase* indexingAndPageScanning = getWidget("indexingAndPageScanning");
	if (indexingAndPageScanning == nullptr)
	{
		indexingAndPageScanning = new LabelReportElement("blockLabel", m_viewFrameBody);
		indexingAndPageScanning->setValue(QObject::tr("Indexing and Page Scanning"));
		m_reportElements["indexingAndPageScanning"] = indexingAndPageScanning;
	}

	indexingAndPageScanning->setSize(percentWidthPixels(45), Common::Helpers::pointsToPixels(18));
	indexingAndPageScanning->setRelativePosition(bodyMargin, Common::Helpers::pointsToPixels(10), getWidget("spacer2"), nullptr);

	addReportItem(Key::StatusCode4xxImage, Key::StatusCode4xx, Key::StatusCode4xxCount, indexingAndPageScanning, nullptr);
	addReportItem(Key::StatusCode5xxImage, Key::StatusCode5xx, Key::StatusCode5xxCount, getWidget(Key::StatusCode4xxCount), nullptr);
	addReportItem(Key::NotIndexedPagesImage, Key::NotIndexedPages, Key::NotIndexedPagesCount, getWidget(Key::StatusCode5xxCount), nullptr);
	addReportItem(Key::ConfiguredCorrectly404Image, Key::ConfiguredCorrectly404, Key::ConfiguredCorrectly404Count, getWidget(ReportDataKeys::NotIndexedPagesCount), nullptr);
	addReportItem(Key::RobotsTxtImage, Key::RobotsTxt, Key::RobotsTxtCount, getWidget(Key::ConfiguredCorrectly404Count), nullptr);
	addReportItem(Key::XmlSitemapImage, Key::XmlSitemap, Key::XmlSitemapCount, getWidget(Key::RobotsTxtCount), nullptr);


	ReportElementBase* technicalFactors = getWidget("technicalFactors");
	if (technicalFactors == nullptr)
	{
		technicalFactors = new LabelReportElement("blockLabel", m_viewFrameBody);
		technicalFactors->setValue(QObject::tr("Technical factors"));
		m_reportElements["technicalFactors"] = technicalFactors;
	}

	technicalFactors->setSize(percentWidthPixels(45), Common::Helpers::pointsToPixels(18));
	technicalFactors->setRelativePosition(percentWidthPixels(52), Common::Helpers::pointsToPixels(10), getWidget("spacer2"), nullptr);

	addReportItem(Key::PagesWithDuplicatedRelCanonicalImage, Key::PagesWithDuplicatedRelCanonical, Key::PagesWithDuplicatedRelCanonicalCount, technicalFactors, nullptr, 52);
	addReportItem(Key::PagesContainsFramesImage, Key::PagesContainsFrames, Key::PagesContainsFramesCount, getWidget(Key::PagesWithDuplicatedRelCanonicalCount), nullptr, 52);
	addReportItem(Key::TooLargePagesImage, Key::TooLargePages, Key::TooLargePagesCount, getWidget(Key::PagesContainsFramesCount), nullptr, 52);

	ReportElementBase* redirections = getWidget("redirections");
	if (redirections == nullptr)
	{
		redirections = new LabelReportElement("blockLabel", m_viewFrameBody);
		redirections->setValue(QObject::tr("Redirections"));
		m_reportElements["redirections"] = redirections;
	}

	redirections->setSize(percentWidthPixels(45), Common::Helpers::pointsToPixels(18));
	redirections->setRelativePosition(bodyMargin, Common::Helpers::pointsToPixels(15), getWidget(Key::XmlSitemapCount), nullptr);

	addReportItem(Key::WwwFixedVersionImage, Key::WwwFixedVersion, Key::WwwFixedVersionCount, redirections, nullptr);
	addReportItem(Key::Redirections302Image, Key::Redirections302, Key::Redirections302Count, getWidget(Key::WwwFixedVersionCount), nullptr);
	addReportItem(Key::Redirections301Image, Key::Redirections301, Key::Redirections301Count, getWidget(Key::Redirections302Count), nullptr);
	addReportItem(Key::LargeAmountRedirectsImage, Key::LargeAmountRedirects, Key::LargeAmountRedirectsCount, getWidget(Key::Redirections301Count), nullptr);
	addReportItem(Key::RefreshMetaTagImage, Key::RefreshMetaTag, Key::RefreshMetaTagCount, getWidget(Key::LargeAmountRedirectsCount), nullptr);
	addReportItem(Key::RelCanonicalPagesImage, Key::RelCanonicalPages, Key::RelCanonicalPagesCount, getWidget(Key::RefreshMetaTagCount), nullptr);


	ReportElementBase* onPage = getWidget("onPage");
	if (onPage == nullptr)
	{
		onPage = new LabelReportElement("blockLabel", m_viewFrameBody);
		onPage->setValue(QObject::tr("On Page"));
		m_reportElements["onPage"] = onPage;
	}

	onPage->setSize(percentWidthPixels(45), Common::Helpers::pointsToPixels(18));
	onPage->setRelativePosition(percentWidthPixels(52), Common::Helpers::pointsToPixels(15), getWidget(Key::XmlSitemapCount), nullptr);

	addReportItem(Key::EmptyTitlesImage, Key::EmptyTitles, Key::EmptyTitlesCount, onPage, nullptr, 52);
	addReportItem(Key::DuplicatedTitlesImage, Key::DuplicatedTitles, Key::DuplicatedTitlesCount, getWidget(Key::EmptyTitlesCount), nullptr, 52);
	addReportItem(Key::TooLongTitlesImage, Key::TooLongTitles, Key::TooLongTitlesCount, getWidget(Key::DuplicatedTitlesCount), nullptr, 52);
	addReportItem(Key::EmptyMetaDescriptionsImage, Key::EmptyMetaDescriptions, Key::EmptyMetaDescriptionsCount, getWidget(Key::TooLongTitlesCount), nullptr, 52);
	addReportItem(Key::EmptyMetaDescriptionsImage, Key::EmptyMetaDescriptions, Key::EmptyMetaDescriptionsCount, getWidget(Key::TooLongTitlesCount), nullptr, 52);
	addReportItem(Key::DuplicatedMetaDescriptionsImage, Key::DuplicatedMetaDescriptions, Key::DuplicatedMetaDescriptionsCount, getWidget(Key::EmptyMetaDescriptionsCount), nullptr, 52);
	addReportItem(Key::TooLongMetaDescriptionsImage, Key::TooLongMetaDescriptions, Key::TooLongMetaDescriptionsCount, getWidget(Key::DuplicatedMetaDescriptionsCount), nullptr, 52);

	ReportElementBase* links = getWidget("links");
	if (links == nullptr)
	{
		links = new LabelReportElement("blockLabel", m_viewFrameBody);
		links->setValue(QObject::tr("Links"));
		m_reportElements["links"] = links;
	}

	links->setSize(percentWidthPixels(45), Common::Helpers::pointsToPixels(18));
	links->setRelativePosition(bodyMargin, Common::Helpers::pointsToPixels(15), getWidget(Key::RelCanonicalPagesCount), nullptr);

	addReportItem(Key::BrokenLinksImage, Key::BrokenLinks, Key::BrokenLinksCount, links, nullptr);
	addReportItem(Key::PagesWithLargeAmountOfLinksImage, Key::PagesWithLargeAmountOfLinks, Key::PagesWithLargeAmountOfLinksCount, getWidget(Key::BrokenLinksCount), nullptr);
	addReportItem(Key::ExternalDofollowLinksImage, Key::ExternalDofollowLinks, Key::ExternalDofollowLinksCount, getWidget(Key::PagesWithLargeAmountOfLinksCount), nullptr);
	addReportItem(Key::TooLongUrlAddressesImage, Key::TooLongUrlAddresses, Key::TooLongUrlAddressesCount, getWidget(Key::ExternalDofollowLinksCount), nullptr);

	ReportElementBase* images = getWidget("images");
	if (images == nullptr)
	{
		images = new LabelReportElement("blockLabel", m_viewFrameBody);
		images->setValue(QObject::tr("Images"));
		m_reportElements["images"] = images;
	}

	images->setSize(percentWidthPixels(45), Common::Helpers::pointsToPixels(18));
	images->setRelativePosition(percentWidthPixels(52), Common::Helpers::pointsToPixels(15), getWidget(Key::RelCanonicalPagesCount), nullptr);

	addReportItem(Key::BrokenImagesImage, Key::BrokenImages, Key::BrokenImagesCount, images, nullptr, 52);
	addReportItem(Key::ImagesWithEmptyAltTextImage, Key::ImagesWithEmptyAltText, Key::ImagesWithEmptyAltTextCount, getWidget(Key::BrokenImagesCount), nullptr, 52);

	addSpacerItem("spacer3", getWidget(Key::TooLongUrlAddressesCount));

	ReportElementBase* creationDate = getWidget(Key::FooterDate);
	if (creationDate == nullptr)
	{
		creationDate = new LabelReportElement("footerLabel", m_viewFrameBody, Qt::AlignLeft | Qt::AlignTop);
		m_reportElements[m_reportDataProvider.placeholder(Key::FooterDate)] = creationDate;
	}
	creationDate->setSize(-1, Common::Helpers::pointsToPixels(15) + bodyMargin);
	creationDate->setRelativePosition(bodyMargin, bodyMargin, getWidget("spacer3"), nullptr);

	ReportElementBase* companyName = getWidget(Key::CompanyName);
	if (companyName == nullptr)
	{
		companyName = new LabelReportElement("footerLabel", m_viewFrameBody, Qt::AlignLeft | Qt::AlignTop);
		m_reportElements[m_reportDataProvider.placeholder(Key::CompanyName)] = companyName;
	}
	companyName->setSize(percentWidthPixels(45), Common::Helpers::pointsToPixels(15) + bodyMargin);
	companyName->setRelativePosition(Common::Helpers::pointsToPixels(5), bodyMargin, getWidget("spacer3"), creationDate);
}

int ReportsPage::percentWidthPixels(int percent) const
{
	return m_viewFrameBody->width() * percent / 100;
}
ReportElementBase* ReportsPage::getWidget(ReportDataKeys key) const
{
	return m_reportElements.value(m_reportDataProvider.placeholder(key), nullptr);
}

ReportElementBase* ReportsPage::getWidget(const QByteArray& key) const
{
	return m_reportElements.value(key, nullptr);
}
}
