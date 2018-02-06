#pragma once

#include "message_box_dialog.h"
#include "requester_wrapper.h"
#include "page_factory.h"


namespace SeoSpider
{

class DataPagesWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);

	void init();
	DataPagesWidget* dataPagesWidget() const noexcept;

signals:
	void resized();
	void moved();
	void windowStateChanged();
	void showDataPage(PageFactory::Page);

public slots:
	void showSitemapCreatorDialog();
	void saveFileAs();
	void openFile();
	void openFileThroughCmd(QString path);
	void saveFileAndClearData();

	void showApplicationSettingsDialog(const QByteArray& settingsPageName = QByteArray());

	void showMessageBoxDialog(const QString& title, 
		const QString& message, 
		MessageBoxDialog::Icon icon, 
		QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	void showDataPagesWidget(PageFactory::Page page);
	
protected:
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void moveEvent(QMoveEvent* event) override;
	virtual void changeEvent(QEvent* event) override;

private:
	void createActions();
	void createAndSetCentralWidget();
	void registerSettingsPages() const;

	QString getSaveFilePath() const;
	void clearDataOnSerializationDone();

private:
	bool m_initialized;

	CrawlerEngine::RequesterWrapper m_requester;

	DataPagesWidget* m_dataPagesWidget;
};

}
