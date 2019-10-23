#include "stdafx.h"
#include "dialog.h"
#include "application.h"
#include "main_window.h"
#include "finally.h"
#include "deferred_call.h"
#include "message_box_dialog.h"

namespace SeoSpider
{

Dialog* Dialog::s_dialog = nullptr;
std::deque<Dialog::DeferredDialogData> Dialog::s_defferedDialogs;

Dialog::Dialog(QWidget* parent)
	: QDialog(parent)
{
}

int Dialog::showMessageBoxDialog(const QString& title, const QString& message,
	QDialogButtonBox::StandardButtons buttons)
{
	INFOLOG << "Show message box with message:" << message;

	MessageBoxDialog* messageBoxDialog = new MessageBoxDialog;
	messageBoxDialog->setWindowTitle(title);
	messageBoxDialog->setMessage(message);
	messageBoxDialog->setStandardButtons(buttons);
	messageBoxDialog->exec();

	return messageBoxDialog->result();
}

void Dialog::showDeferredMessageBoxDialog(const QString& title, const QString& message,
	QDialogButtonBox::StandardButtons buttons)
{
	if (shownDialog() == nullptr)
	{
		showMessageBoxDialog(title, message, buttons);
		return;
	}


	const DeferredDialogData data = { title, message, buttons };

	// remove the same dialog in the queue if exists and append a new one in the end
	s_defferedDialogs.erase(std::remove(s_defferedDialogs.begin(), s_defferedDialogs.end(), data), s_defferedDialogs.end());
	s_defferedDialogs.push_back(data);
}

void Dialog::open()
{
	exec();
}

int Dialog::exec()
{
	s_dialog = this;
	Common::Finally finalObject([]() { s_dialog = nullptr; });

	theApp->mainWindow()->showShadedOverlay();

	const int result = QDialog::exec();

	theApp->mainWindow()->hideShadedOverlay();

	DeferredCall::call([this]()
	{
		emit dialogClosed();
		DeferredCall::call([this]()
		{
			for (auto it = s_defferedDialogs.begin(); it != s_defferedDialogs.end(); ++it)
			{
				const DeferredDialogData& data = *it;
				showMessageBoxDialog(data.title, data.message, data.buttons);
			}
			s_defferedDialogs.clear();
		});
	});

	return result;
}

Dialog* Dialog::shownDialog()
{
	return s_dialog;
}

}