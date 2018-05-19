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

	VERIFY(connect(m_ui->downloadNowButton, &QPushButton::clicked, this, &UpdateLoaderDialog::onUpdateNowClicked));
	VERIFY(connect(m_ui->downloadLaterButton, &QPushButton::clicked, this, &UpdateLoaderDialog::onUpdateLaterClicked));

	ShadowDecorationFrame* shadowFrame = new ShadowDecorationFrame(internalFrame, this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(shadowFrame);
}

void UpdateLoaderDialog::showEvent(QShowEvent* event)
{
	WidgetHelpers::moveWidgetToHostCenter(this);

	theApp->mainWindow()->setDisabled(true);

	setEnabled(true);

	QFrame::showEvent(event);
}

void UpdateLoaderDialog::onUpdateNowClicked()
{
	if (QProcess::startDetached("Uninstall SeoSpider.exe", { "--updater" }))
	{
		theApp->quit();
	}
}

void UpdateLoaderDialog::onUpdateLaterClicked()
{
	closeDialog();
}

void UpdateLoaderDialog::closeDialog() noexcept
{
	theApp->mainWindow()->setDisabled(false);

	close();
}

}