#include "reports_page.h"
#include "application.h"
#include "preferences.h"
#include "crawler.h"

namespace SeoSpider
{

ReportsPage::ReportsPage(QWidget* parent)
	: QFrame(parent)
	, m_webEngineView(new QWebEngineView(this))
	, m_reportDataProvider(theApp->crawler()->sequencedDataCollection())
	, m_updateTimerId(0)
{
	theApp->installEventFilter(this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_webEngineView);
	layout->setMargin(0);
		
	setReportType(ReportTypeBrief);

	VERIFY(connect(theApp->preferences(), &Preferences::companyNameChanged, this, &ReportsPage::updateContent));
}

void ReportsPage::setReportType(ReportType reportType)
{
	m_webEngineView->setHtml(reportMaketContent(reportType));

	m_reportType = reportType;
}

bool ReportsPage::eventFilter(QObject* object, QEvent* event)
{
#ifndef PRODUCTION

	if (object == qobject_cast<QObject*>(this) && event->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

		if (QKeySequence("F7") == QKeySequence(keyEvent->key()))
		{
			setDebugReportType(m_reportType);

			return true;
		}
	}

#endif

	return false;
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
			content.replace("((" + placeholder + "))", value.toString().toLatin1());
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

#ifndef PRODUCTION

void ReportsPage::setDebugReportType(ReportType reportType)
{
	m_webEngineView->setHtml(debugReportMaketContent(reportType));
}

QByteArray ReportsPage::debugReportMaketContent(ReportType reportType) const
{
	QFile reportMaket;
	QFile maketStyle(QStringLiteral("C:/maket_style.css"));

	switch (reportType)
	{
		case ReportTypeBrief:
		{
			reportMaket.setFileName(QStringLiteral("C:/brief_report_maket.html"));
			break;
		}
		case ReportTypeDetailed:
		{
			reportMaket.setFileName(QStringLiteral("C:/detailed_report_maket.html"));
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

#endif

}