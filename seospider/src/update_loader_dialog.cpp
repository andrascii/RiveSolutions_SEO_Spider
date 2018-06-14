#include "update_loader_dialog.h"
#include "application.h"
#include "widget_helpers.h"
#include "main_window.h"
#include "update_helpers.h"
#include "shadow_decoration_frame.h"

namespace SeoSpider
{

UpdateLoaderDialog::UpdateLoaderDialog(QWidget* parent)
	: QFrame(parent)
	, m_ui(new Ui_UpdateLoaderDialogContent)
{
	QFrame* internalFrame = new QFrame;
	m_ui->setupUi(internalFrame);

	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	setWindowModality(Qt::ApplicationModal);
	setAttribute(Qt::WA_DeleteOnClose, true);

	VERIFY(connect(m_ui->downloadNowButton, &QPushButton::clicked, this, &UpdateLoaderDialog::onDownloadNowClicked));
	VERIFY(connect(m_ui->downloadLaterButton, &QPushButton::clicked, this, &UpdateLoaderDialog::onDownloadLaterClicked));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(new ShadowDecorationFrame(internalFrame, this));
}

void UpdateLoaderDialog::showEvent(QShowEvent* event)
{
	theApp->mainWindow()->showShadedOverlay();

	WidgetHelpers::moveWidgetToHostCenter(this);

	theApp->mainWindow()->setDisabled(true);

	setEnabled(true);
	QFrame::showEvent(event);
}

void UpdateLoaderDialog::hideEvent(QHideEvent* event)
{
	theApp->mainWindow()->hideShadedOverlay();
	QFrame::hideEvent(event);
}

void UpdateLoaderDialog::closeEvent(QCloseEvent*)
{
	theApp->mainWindow()->setDisabled(false);
	theApp->quit();
}

void UpdateLoaderDialog::onDownloadNowClicked()
{
	QString repositoryUrl = QString(UpdateHelpers::actualVersionFileUrl());
	repositoryUrl.truncate(repositoryUrl.lastIndexOf("/"));

	INFOLOG << "Launch installer with url:" << repositoryUrl;

	if (QProcess::startDetached("Uninstall SeoSpider.exe", { "--updater", "--setTempRepository", repositoryUrl }))
	{
		theApp->quit();
	}
}

void UpdateLoaderDialog::onDownloadLaterClicked()
{
	closeDialog();
}

void UpdateLoaderDialog::closeDialog() noexcept
{
	theApp->mainWindow()->setDisabled(false);
	theApp->quit();

	close();
}

}