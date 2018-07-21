#pragma once

#include "report_data_provider.h"

namespace SeoSpider
{

class IReportExporter;
class ReportElementBase;

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
	~ReportsPage();

	void updateLayout(ReportType report, const bool rebuildLayout);
	void setReportType(ReportType reportType);

	Q_SLOT void exportToPdf();

protected:
	virtual void timerEvent(QTimerEvent* event) override;
	virtual void showEvent(QShowEvent* event) override;
	virtual void hideEvent(QHideEvent* event) override;

private:
	QByteArray reportMaketContent(ReportType reportType) const;

	void changePlaceholderInContent(const QByteArray& placeholder, const QVariant& value, QByteArray& content) const;

	Q_SLOT void updateContent();
	Q_SLOT void crawlerStateChangedSlot(int state);
	Q_SLOT void crawlerDataCleared();

	void doExport(IReportExporter* exporter) const;

	void buildLayout(ReportType reportType);
	void buildLayoutBrief();
	int percentWidthPixels(int percent) const;
	ReportElementBase* getWidget(ReportDataKeys key) const;
	ReportElementBase* getWidget(const QByteArray& key) const;

private:
	QStackedWidget* m_stackedWidget;
	QLabel* m_placeHolderLabel;
	ReportType m_reportType;
	ReportDataProvider m_reportDataProvider;
	int m_updateTimerId;

	QFrame* m_viewFrame;
	QFrame* m_viewFrameBody;
	QScrollArea* m_scrollArea;
	QMap<QByteArray, ReportElementBase*> m_reportElements;
};

}
