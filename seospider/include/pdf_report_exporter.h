#pragma once
#include "ireport_exporter.h"

namespace SeoSpider
{

class PdfReportExporter : public IReportExporter
{
	Q_DECLARE_TR_FUNCTIONS(PdfReportExporter)
public:
	virtual QString description() const override;
	virtual QString fileMask() const override;
	virtual QString ext() const override;

	virtual void doExport(QIODevice* device, ReportsPage::ReportType reportType, 
		const ReportDataProvider* provider) const override;

private:
	void doExportBriefReport(QIODevice* device, const ReportDataProvider* provider) const;

	QRect shiftedToIconRect(const QRect& rect) const;
	QRect iconRect(const QRect& rect) const;
	QImage providerIcon(const ReportDataProvider* provider, ReportDataKeys key) const;

	void drawOneLineInfo(const ReportDataProvider* provider, QPainter* painter, const QRect& lineRect,
		ReportDataKeys keyIcon, ReportDataKeys keyText, ReportDataKeys keyCount, const QFont* = nullptr) const;

	void drawOneLineInfo(const ReportDataProvider* provider, QPainter* painter, const QRect& lineRect,
		ReportDataKeys keyText, ReportDataKeys keyCount, const QFont* = nullptr) const;

	void drawOneLineInfo(QPainter* painter, const QRect& lineRect,
		const QImage& icon, const QString& text, const QString& count, const QFont* = nullptr) const;

	void drawInfoBlockHeader(QPainter* painter, const QRect& lineRect, const QString& header) const;

	void adjustColumnsRects(QRect& firstColumnRect, QRect& secondColumnRect, int extraRowSpacing) const;
};

}