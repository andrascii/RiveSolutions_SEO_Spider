#include "dialog.h"
#include "application.h"
#include "main_window.h"
#include "finally.h"
#include "deferred_call.h"

namespace SeoSpider
{

Dialog* Dialog::s_dialog = nullptr;

Dialog::Dialog(QWidget* parent)
	: QDialog(parent)
{
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
	});

	return result;
}

Dialog* Dialog::shownDialog()
{
	return s_dialog;
}

}