#include "reports_page.h"
#include "application.h"

namespace SeoSpider
{

ReportsPage::ReportsPage(QWidget* parent)
	: QFrame(parent)
	, m_webEngineView(new QWebEngineView(this))
{
	theApp->installEventFilter(this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_webEngineView);
	layout->setMargin(0);

	setReportType(ReportTypeBrief);
}

void ReportsPage::setReportType(ReportType reportType)
{
	m_webEngineView->setHtml(reportMaketContent(reportType));

	m_reportType = reportType;
}

bool ReportsPage::eventFilter(QObject* object, QEvent* event)
{
#ifndef PRODUCTION

	if (event->type() == QEvent::KeyPress)
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
	changeMarkerInContent("stylesheet", maketStyle.readAll(), maketContent);

	return maketContent;
}

void ReportsPage::changeMarkerInContent(const QByteArray& marker, const QByteArray& value, QByteArray& content) const
{
	content.replace("((" + marker + "))", value);
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
	changeMarkerInContent("stylesheet", maketStyle.readAll(), maketContent);

	return maketContent;
}

#endif

}