#include "stdafx.h"
#include "audit_report_page.h"
#include "reports_page.h"
#include "svg_renderer.h"
#include "statistic_counter.h"

namespace SeoSpider
{

AuditReportPage::AuditReportPage(QWidget* parent)
	: QFrame(parent)
	, m_reportsPage(new ReportsPage(this))
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	layout->addWidget(m_reportsPage);

	createHeaderActionWidgets();
}

QWidget* AuditReportPage::widget() const
{
	return const_cast<AuditReportPage*>(this);
}

QString AuditReportPage::name() const
{
	return tr("Audit Report");
}

QIcon AuditReportPage::icon() const
{
	QIcon icon;
	icon.addPixmap(SvgRenderer::render(":/images/report-page-icon.svg", 15, 15), QIcon::Normal, QIcon::On);
	icon.addPixmap(SvgRenderer::render(":/images/report-page-icon.svg", 15, 15), QIcon::Active, QIcon::On);
	icon.addPixmap(SvgRenderer::render(":/images/report-page-icon.svg", 15, 15), QIcon::Active, QIcon::Off);
	icon.addPixmap(SvgRenderer::render(":/images/report-page-icon.svg", 15, 15), QIcon::Normal, QIcon::Off);

	return icon;
}

IPage::Type AuditReportPage::type() const
{
	return IPage::AuditReportPage;
}

void AuditReportPage::createHeaderActionWidgets()
{
	QAction* saveToPdfAction = new QAction(tr("Export to PDF"), this);
	saveToPdfAction->setIcon(SvgRenderer::render(":/images/pdf.svg", 25, 25));
	VERIFY(connect(saveToPdfAction, &QAction::triggered, m_reportsPage, &ReportsPage::exportToPdf));

	AbstractPage::addAction(saveToPdfAction);
}

void AuditReportPage::showEvent(QShowEvent* event)
{
	QFrame::showEvent(event);

	Common::StatisticCounter showCounter("AuditReportPageShowCounter");
	showCounter.increment();
}

}
