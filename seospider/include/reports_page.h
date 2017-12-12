#pragma once

namespace SeoSpider
{

class ReportsPage : public QFrame
{
public:
	enum ReportType
	{
		ReportTypeBrief,
		ReportTypeDetailed
	};

	ReportsPage(QWidget* parent = nullptr);

	void setReportType(ReportType reportType);

protected:
	virtual bool eventFilter(QObject* object, QEvent* event) override;

private:
	QByteArray reportMaketContent(ReportType reportType) const;

	void changeMarkerInContent(const QByteArray& marker, const QByteArray& value, QByteArray& content) const;

#ifndef PRODUCTION

	void setDebugReportType(ReportType reportType);
	QByteArray debugReportMaketContent(ReportType reportType) const;

#endif

private:
	QWebEngineView* m_webEngineView;
	ReportType m_reportType;
};

}