#pragma once

#include "message_box_dialog.h"
#include "requester_wrapper.h"

namespace SeoSpider
{

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);

	void init();

public slots:
	void showApplicationSettingsWidget(const QByteArray& settingsPageName = QByteArray());
	void showMessageBoxDialog(const QString& title, 
		const QString& message, 
		MessageBoxDialog::Icon icon, 
		QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

private:
	void createActions();
	void createAndSetCentralWidget();
	void registerSettingsPages() const;

private:
	bool m_initialized;

	CrawlerEngine::RequesterWrapper m_requester;
};

}