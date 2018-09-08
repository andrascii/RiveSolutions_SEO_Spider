#pragma once

#include "ui_application_settings_dialog.h"
#include "dialog.h"

namespace SeoSpider
{

class ApplicationSettingsDialog : public Dialog
{
	Q_OBJECT

public:
	ApplicationSettingsDialog(QWidget* parent = nullptr);
	~ApplicationSettingsDialog();

	void setCurrentPage(const QByteArray& settingsPageName);

private slots:
	void applyChanges();
	void okButtonClicked();
	void cancelButtonClicked();
	void somethingChangedSlot();
	void reloadSettingsSlot();
	void onCrawlerStarted();
	void onCrawlerFinished();
	void currentSettingsPageChanged(int index);

private:
	void initialize();
	void restoreChangedValues();
	virtual void showEvent(QShowEvent* event) override;
	virtual void hideEvent(QHideEvent* event) override;
	virtual void closeEvent(QCloseEvent* event) override;

private:
	Ui::ApplicationSettingsDialog m_ui;
	QMap<QByteArray, int> m_pageIndex;
	bool m_somethingChanged;
};

}