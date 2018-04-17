#pragma once

#include "ui_updates_loader_dialog_content.h"
#include "requester_wrapper.h"
#include "url.h"

namespace SeoSpider
{

class UpdateLoader;

class UpdateLoaderDialog : public QFrame
{
	Q_OBJECT

private:
	enum class UnitType
	{
		UnitTypeBytes,
		UnitTypeKB,
		UnitTypeMB
	};

public:
	UpdateLoaderDialog(const QString& downloadLink, QWidget* parent = nullptr);

signals:
	void updateDownloaded(const QString& filepath);

protected:
	virtual void showEvent(QShowEvent* event) override;

private slots:
	void onDownloadNowClicked();
	void onDownloadLaterClicked();
	void onAboutDownloadProgress(quint64 bytesReceived, quint64 bytesTotal, double downloadSpeed);
	void onUpdatesDownloadingFinished(const QString& filepath);

private:
	void closeDialog() noexcept;
	std::tuple<double, double, QString> downloadStatus(quint64 bytesReceived, quint64 bytesTotal) const;

	UnitType unit(quint64 bytesCount) const noexcept;
	QString unitToString(UnitType unitType) const;

	double downloadPercents(quint64 bytesReceived, quint64 bytesTotal) const;
	QString downloadStatusString(quint64 bytesReceived, quint64 bytesTotal) const;
	QString downloadSpeedString(double downloadSpeed) const;
	double fromUnitToUnit(double value, UnitType from, UnitType to) const;

private:
	Ui_UpdatesLoaderDialogContent* m_ui;
	CrawlerEngine::Url m_downloadLink;
	UpdateLoader* m_updateLoader;
};

}