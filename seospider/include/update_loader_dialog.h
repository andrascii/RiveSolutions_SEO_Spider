#pragma once

#include "ui_update_loader_dialog_content.h"
#include "requester_wrapper.h"
#include "url.h"

namespace SeoSpider
{

class UpdateLoader;

class UpdateLoaderDialog : public QFrame
{
	Q_OBJECT

public:
	UpdateLoaderDialog(QWidget* parent = nullptr);

signals:
	void updateDownloaded(const QString& filepath);

protected:
	virtual void showEvent(QShowEvent* event) override;
	virtual void hideEvent(QHideEvent* event) override;
	virtual void closeEvent(QCloseEvent *event) override;

private slots:
	void onDownloadNowClicked();
	void onDownloadLaterClicked();

private:
	void closeDialog() noexcept;

private:
	Ui_UpdateLoaderDialogContent* m_ui;
};

}