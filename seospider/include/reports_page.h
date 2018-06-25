#pragma once

#include "report_data_provider.h"

namespace SeoSpider
{

class IReportExporter;

class ReportsPage : public QFrame
{
	Q_OBJECT

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
	virtual void timerEvent(QTimerEvent* event) override;
	virtual void showEvent(QShowEvent* event) override;
	virtual void hideEvent(QHideEvent* event) override;

private:
	QByteArray reportMaketContent(ReportType reportType) const;

	void changePlaceholderInContent(const QByteArray& placeholder, const QVariant& value, QByteArray& content) const;

	Q_SLOT void updateContent();

	Q_SLOT void exportToPdf();

	Q_SLOT void crawlerStateChangedSlot(int state);

	Q_SLOT void crawlerDataCleared();

	void doExport(IReportExporter* exporter) const;

#ifndef PRODUCTION

	void setDebugReportType(ReportType reportType);
	QByteArray debugReportMaketContent(ReportType reportType) const;

#endif

private:
	QStackedWidget* m_stackedWidget;
	QLabel* m_placeHolderLabel;
	QWebEngineView* m_webEngineView;
	ReportType m_reportType;
	ReportDataProvider m_reportDataProvider;
	int m_updateTimerId;

	QAction* m_saveToPdfAction;
};

}
