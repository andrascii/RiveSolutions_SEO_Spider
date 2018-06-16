#include "dialog.h"
#include "application.h"
#include "main_window.h"

namespace SeoSpider
{

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
	theApp->mainWindow()->showShadedOverlay();

	int result = QDialog::exec();

	theApp->mainWindow()->hideShadedOverlay();

	return result;
}

}