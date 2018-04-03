#pragma once

#include "ui_application_settings_widget.h"

namespace SeoSpider
{

class ApplicationSettingsWidget : public QDialog
{
	Q_OBJECT

public:
	ApplicationSettingsWidget(QWidget* parent = nullptr);
	~ApplicationSettingsWidget();

	void setCurrentPage(const QByteArray& settingsPageName);

private slots:
	void applyChanges();
	void okButtonClicked();
	void cancelButtonClicked();
	void somethingChangedSlot();
	void reloadSettingsSlot();
	void onCrawlerStarted();
	void onCrawlerFinished();

private:
	void initialize();
	void restoreChangedValues();
	virtual void showEvent(QShowEvent* event) override;
	virtual void hideEvent(QHideEvent* event) override;

private:
	Ui::ApplicationSettingsWidget m_ui;
	QMap<QByteArray, int> m_pageIndex;
	bool m_somethingChanged;
};

}