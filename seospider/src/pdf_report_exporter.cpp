#include "stdafx.h"
#include "pdf_report_exporter.h"
#include "report_data_provider.h"
#include "statistic_counter.h"

namespace SeoSpider
{

QString PdfReportExporter::description() const
{
	return tr("Export to PDF");
}

QString PdfReportExporter::fileMask() const
{
	return tr("PDF Files (*.pdf)");
}

QString PdfReportExporter::ext() const
{
	return QString(".pdf");
}

void PdfReportExporter::doExport(QIODevice* device, 
	ReportsPage::ReportType reportType, const ReportDataProvider* provider) const
{
	StatisticCounter exportCounter(QString("ExportReportToPdfCounter"));
	exportCounter.increment();

	switch (reportType)
	{
	case ReportsPage::ReportTypeBrief:
		doExportBriefReport(device, provider);
		break;
	default:
		ASSERT(!"Unsupported report type");
	}
}

void PdfReportExporter::doExportBriefReport(QIODevice* device, const ReportDataProvider* provider) const
{
	QPdfWriter writer(device);
	writer.setPageSize(QPagedPaintDevice::A4);
	writer.setPageMargins(QMargins(30, 30, 30, 30));

	QPainter painter(&writer);
	painter.setPen(Qt::black);
	painter.setFont(QFont("Helvetica", 9));

	const QRect rect = painter.viewport();
	const QString testText = "RiveSolutions SeoSpider Report";

	painter.drawText(rect, Qt::AlignRight | Qt::AlignTop, testText);

	const int startDataHeaderYCoord = 1500;
	const QRect headerRect(0, startDataHeaderYCoord, rect.width(), 400);

	const QString headerText =
		provider->data(ReportDataKeys::SiteLink).toString() + QString(", ") +
		provider->data(ReportDataKeys::Date).toString();

	painter.setFont(QFont("Helvetica", 20));
	painter.drawText(headerRect, Qt::AlignCenter | Qt::AlignTop, headerText);

	const int rowSpacing = 300;
	const int columnLeftMargin = 400;
	const int columnRightMargin = 200;
	const int startDataYSpace = 1000;

	QRect firstColumnRect(columnLeftMargin,
		startDataHeaderYCoord + startDataYSpace,
		rect.width() / 2 - columnLeftMargin - columnRightMargin,
		400);

	QRect secondColumnRect(rect.width() / 2 + columnLeftMargin,
		startDataHeaderYCoord + startDataYSpace,
		rect.width() / 2 - columnLeftMargin - columnRightMargin,
		400);

	// site image
	QPixmap image = qvariant_cast<QPixmap>(provider->data(ReportDataKeys::SiteShortImage));
	const QRect imageRect(firstColumnRect.topLeft(), QSize(firstColumnRect.width(), 2000));
	if (image.isNull())
	{
		// TODO: replace with some other image
		//image = QPixmap(":/images/oops.jpg");
		painter.save();
		painter.fillRect(imageRect, QBrush(Qt::lightGray));
		painter.setBrush(QBrush(Qt::gray));
		painter.drawText(imageRect, Qt::AlignCenter | Qt::AlignTop, tr("No image"));
		painter.restore();
	}
	else
	{
		image = image.scaledToWidth(firstColumnRect.width(), Qt::FastTransformation);
		QPixmap cropped = image.copy(QRect(QPoint(0, 0), imageRect.size()));

		painter.drawImage(imageRect, cropped.toImage());
	}

	firstColumnRect = firstColumnRect.adjusted(0, imageRect.height(), 0, imageRect.height());


	// summary info
	QFont summaryFont("Helvetica", 14);
	const int summaryRowSpacing = 400;
	drawOneLineInfo(provider, &painter, secondColumnRect, ReportDataKeys::FoundProblemsExceptInfo, ReportDataKeys::FoundProblemsExceptInfoCount, &summaryFont);

	secondColumnRect = secondColumnRect.adjusted(0, summaryRowSpacing, 0, summaryRowSpacing);
	drawOneLineInfo(provider, &painter, secondColumnRect, ReportDataKeys::Errors, ReportDataKeys::ErrorsCount, &summaryFont);

	secondColumnRect = secondColumnRect.adjusted(0, summaryRowSpacing, 0, summaryRowSpacing);
	drawOneLineInfo(provider, &painter, secondColumnRect, ReportDataKeys::Warnings, ReportDataKeys::WarningsCount, &summaryFont);

	const int spacingBetweenBlocks = 800;

	adjustColumnsRects(firstColumnRect, secondColumnRect, spacingBetweenBlocks);

	// Indexing and Page Scanning
	drawInfoBlockHeader(&painter, firstColumnRect, tr("Indexing and Page Scanning"));

	firstColumnRect = firstColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, firstColumnRect, ReportDataKeys::StatusCode4xxImage,
		ReportDataKeys::StatusCode4xx, ReportDataKeys::StatusCode4xxCount);

	firstColumnRect = firstColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, firstColumnRect, ReportDataKeys::StatusCode5xxImage,
		ReportDataKeys::StatusCode5xx, ReportDataKeys::StatusCode5xxCount);

	firstColumnRect = firstColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, firstColumnRect, ReportDataKeys::NotIndexedPagesImage,
		ReportDataKeys::NotIndexedPages, ReportDataKeys::NotIndexedPagesCount);

	firstColumnRect = firstColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, firstColumnRect, ReportDataKeys::ConfiguredCorrectly404Image,
		ReportDataKeys::ConfiguredCorrectly404, ReportDataKeys::ConfiguredCorrectly404Count);

	firstColumnRect = firstColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, firstColumnRect, ReportDataKeys::RobotsTxtImage,
		ReportDataKeys::RobotsTxt, ReportDataKeys::RobotsTxtCount);

	firstColumnRect = firstColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, firstColumnRect, ReportDataKeys::XmlSitemapImage,
		ReportDataKeys::XmlSitemap, ReportDataKeys::XmlSitemapCount);

	// Technical factors
	drawInfoBlockHeader(&painter, secondColumnRect, tr("Technical Factors"));

	secondColumnRect = secondColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, secondColumnRect, ReportDataKeys::PagesWithDuplicatedRelCanonicalImage,
		ReportDataKeys::PagesWithDuplicatedRelCanonical, ReportDataKeys::PagesWithDuplicatedRelCanonicalCount);

	secondColumnRect = secondColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, secondColumnRect, ReportDataKeys::PagesContainsFramesImage,
		ReportDataKeys::PagesContainsFrames, ReportDataKeys::PagesContainsFramesCount);

	//secondColumnRect = secondColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	//drawOneLineInfo(provider, &painter, secondColumnRect, ReportDataKeys::PagesWithHtmlErrorsImage,
	//	ReportDataKeys::PagesWithHtmlErrors, ReportDataKeys::PagesWithHtmlErrorsCount);

	//secondColumnRect = secondColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	//drawOneLineInfo(provider, &painter, secondColumnRect, ReportDataKeys::PagesWithCssErrorsWarningsImage,
	//	ReportDataKeys::PagesWithCssErrorsWarnings, ReportDataKeys::PagesWithCssErrorsWarningsCount);

	secondColumnRect = secondColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, secondColumnRect, ReportDataKeys::TooLargePagesImage,
		ReportDataKeys::TooLargePages, ReportDataKeys::TooLargePagesCount);



	adjustColumnsRects(firstColumnRect, secondColumnRect, spacingBetweenBlocks);
	// Redirections
	drawInfoBlockHeader(&painter, firstColumnRect, tr("Technical Factors"));

	firstColumnRect = firstColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, firstColumnRect, ReportDataKeys::WwwFixedVersionImage,
		ReportDataKeys::WwwFixedVersion, ReportDataKeys::WwwFixedVersionCount);

	firstColumnRect = firstColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, firstColumnRect, ReportDataKeys::Redirections302Image,
		ReportDataKeys::Redirections302, ReportDataKeys::Redirections302Count);

	firstColumnRect = firstColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, firstColumnRect, ReportDataKeys::Redirections301Image,
		ReportDataKeys::Redirections301, ReportDataKeys::Redirections301Count);

	firstColumnRect = firstColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, firstColumnRect, ReportDataKeys::LargeAmountRedirectsImage,
		ReportDataKeys::LargeAmountRedirects, ReportDataKeys::LargeAmountRedirectsCount);

	firstColumnRect = firstColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, firstColumnRect, ReportDataKeys::RefreshMetaTagImage,
		ReportDataKeys::RefreshMetaTag, ReportDataKeys::RefreshMetaTagCount);

	firstColumnRect = firstColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, firstColumnRect, ReportDataKeys::RelCanonicalPagesImage,
		ReportDataKeys::RelCanonicalPages, ReportDataKeys::RelCanonicalPagesCount);


	// On Page
	drawInfoBlockHeader(&painter, secondColumnRect, tr("On Page"));

	secondColumnRect = secondColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, secondColumnRect, ReportDataKeys::EmptyTitlesImage,
		ReportDataKeys::EmptyTitles, ReportDataKeys::EmptyTitlesCount);

	secondColumnRect = secondColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, secondColumnRect, ReportDataKeys::DuplicatedTitlesImage,
		ReportDataKeys::DuplicatedTitles, ReportDataKeys::DuplicatedTitlesCount);

	secondColumnRect = secondColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, secondColumnRect, ReportDataKeys::TooLongTitlesImage,
		ReportDataKeys::TooLongTitles, ReportDataKeys::TooLongTitlesCount);

	secondColumnRect = secondColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, secondColumnRect, ReportDataKeys::EmptyMetaDescriptionsImage,
		ReportDataKeys::EmptyMetaDescriptions, ReportDataKeys::EmptyMetaDescriptionsCount);

	secondColumnRect = secondColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, secondColumnRect, ReportDataKeys::DuplicatedMetaDescriptionsImage,
		ReportDataKeys::DuplicatedMetaDescriptions, ReportDataKeys::DuplicatedMetaDescriptionsCount);

	secondColumnRect = secondColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, secondColumnRect, ReportDataKeys::TooLongMetaDescriptionsImage,
		ReportDataKeys::TooLongMetaDescriptions, ReportDataKeys::TooLongMetaDescriptionsCount);
	

	adjustColumnsRects(firstColumnRect, secondColumnRect, spacingBetweenBlocks);
	// Links
	drawInfoBlockHeader(&painter, firstColumnRect, tr("Links"));

	firstColumnRect = firstColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, firstColumnRect, ReportDataKeys::BrokenLinksImage,
		ReportDataKeys::BrokenLinks, ReportDataKeys::BrokenLinksCount);

	firstColumnRect = firstColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, firstColumnRect, ReportDataKeys::PagesWithLargeAmountOfLinksImage,
		ReportDataKeys::PagesWithLargeAmountOfLinks, ReportDataKeys::PagesWithLargeAmountOfLinksCount);

	firstColumnRect = firstColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, firstColumnRect, ReportDataKeys::ExternalDofollowLinksImage,
		ReportDataKeys::ExternalDofollowLinks, ReportDataKeys::ExternalDofollowLinksCount);

	firstColumnRect = firstColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, firstColumnRect, ReportDataKeys::TooLongUrlAddressesImage,
		ReportDataKeys::TooLongUrlAddresses, ReportDataKeys::TooLongUrlAddressesCount);


	// Images
	drawInfoBlockHeader(&painter, secondColumnRect, tr("Images"));

	secondColumnRect = secondColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, secondColumnRect, ReportDataKeys::BrokenImagesImage,
		ReportDataKeys::BrokenImages, ReportDataKeys::BrokenImagesCount);

	secondColumnRect = secondColumnRect.adjusted(0, rowSpacing, 0, rowSpacing);
	drawOneLineInfo(provider, &painter, secondColumnRect, ReportDataKeys::ImagesWithEmptyAltTextImage,
		ReportDataKeys::ImagesWithEmptyAltText, ReportDataKeys::ImagesWithEmptyAltTextCount);
}

QRect PdfReportExporter::shiftedToIconRect(const QRect& rect) const
{
	return rect.adjusted(350, 0, 0, 0);
}

QRect PdfReportExporter::iconRect(const QRect& rect) const
{
	return QRect(rect.x() + 50, rect.top() + 50, 150, 150);
}

QImage PdfReportExporter::providerIcon(const ReportDataProvider* provider, ReportDataKeys key) const
{
	return qvariant_cast<QPixmap>(provider->data(key)).toImage();

}

void PdfReportExporter::drawOneLineInfo(const ReportDataProvider* provider, QPainter* painter, const QRect& lineRect,
	ReportDataKeys keyIcon, ReportDataKeys keyText, ReportDataKeys keyCount, const QFont* customFont) const
{
	const QImage icon = providerIcon(provider, keyIcon);
	const QString text = provider->data(keyText).toString();
	const QString count = provider->data(keyCount).toString();
	
	drawOneLineInfo(painter, lineRect, icon, text, count, customFont);
}

void PdfReportExporter::drawOneLineInfo(const ReportDataProvider* provider, QPainter* painter, const QRect& lineRect,
	ReportDataKeys keyText, ReportDataKeys keyCount, const QFont* customFont) const
{
	const QString text = provider->data(keyText).toString();
	const QString count = provider->data(keyCount).toString();

	drawOneLineInfo(painter, lineRect, QImage(), text, count, customFont);
}

void PdfReportExporter::drawOneLineInfo(QPainter* painter, const QRect& lineRect, const QImage& icon,
	const QString& text, const QString& count, const QFont* customFont) const
{
	painter->save();

	if (customFont == nullptr)
	{
		const QFont font = QFont("Helvetica", 9);
		painter->setFont(font);
	}
	else
	{
		painter->setFont(*customFont);
	}
	

	if (!icon.isNull())
	{
		painter->drawImage(iconRect(lineRect), icon);
	}
	
	painter->drawText(!icon.isNull() ? shiftedToIconRect(lineRect) : lineRect, Qt::AlignLeft | Qt::AlignTop, text);
	painter->drawText(shiftedToIconRect(lineRect), Qt::AlignRight | Qt::AlignTop, count);

	painter->restore();
}

void PdfReportExporter::drawInfoBlockHeader(QPainter* painter, const QRect& lineRect, const QString& header) const
{
	painter->save();
	
	QFont font = QFont("Helvetica", 10, 2);
	font.setBold(true);
	painter->setFont(font);

	painter->drawText(lineRect, Qt::AlignLeft | Qt::AlignTop, header);

	painter->restore();
}

void PdfReportExporter::adjustColumnsRects(QRect& firstColumnRect, QRect& secondColumnRect, int extraRowSpacing) const
{
	const int y = qMax(firstColumnRect.y(), secondColumnRect.y()) + extraRowSpacing;

	firstColumnRect = QRect(firstColumnRect.x(), y, firstColumnRect.width(), firstColumnRect.height());
	secondColumnRect = QRect(secondColumnRect.x(), y, secondColumnRect.width(), secondColumnRect.height());
}

}
