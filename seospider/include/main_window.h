#pragma once

#include "message_box_dialog.h"
#include "requester_wrapper.h"
#include "page_factory.h"


namespace SeoSpider
{

class ContentFrame;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);

	void init();
	ContentFrame* contentFrame() const noexcept;

signals:
	void resized();
	void moved();
	void windowStateChanged();
	void showPage(PageFactory::Page);

public slots:
	void showSitemapCreatorDialog();
	void saveFile();
	void saveFileAs();
	void openFile();
	void closeFile();
	void saveFileAndClearData();
	void exportFilterData();
	void onChangeGroupingAuditInfo(QAction* action);

	void showApplicationSettingsDialog(const QByteArray& settingsPageName = QByteArray());

	void showMessageBoxDialog(const QString& title, 
		const QString& message, 
		MessageBoxDialog::Icon icon, 
		QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	void showContentFramePage(PageFactory::Page page);

protected:
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void moveEvent(QMoveEvent* event) override;
	virtual void changeEvent(QEvent* event) override;

private:
	void createActions();
	void createHeaderPageDependentActions();
	void createAndSetCentralWidget();
	void registerSettingsPages() const;

	QString getSaveFilePath() const;
	void clearDataOnSerializationDone();

private slots:
	void onCrawlerSessionCreated();
	void onCrawlerSessionDestroyed();

private:
	bool m_initialized;

	CrawlerEngine::RequesterWrapper m_requester;

	ContentFrame* m_contentFrame;
};

}
