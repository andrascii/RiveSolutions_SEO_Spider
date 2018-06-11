#include "dialog.h"
#include "dialog_container.h"
#include "application.h"
#include "main_window.h"

namespace SeoSpider
{

Dialog::Dialog(QWidget* parent)
	: QFrame(parent)
{
}

QObject* Dialog::qobject() const
{
	return const_cast<Dialog*>(this);
}

void Dialog::accept()
{
	done(QDialog::Accepted);
}

void Dialog::reject()
{
	done(QDialog::Rejected);
}

void Dialog::open()
{
	theApp->mainWindow()->showShadedOverlay();

	DialogContainer::instance().openDialog(this);

	theApp->mainWindow()->hideShadedOverlay();
}

void Dialog::done(int r)
{
	m_dialogCode = static_cast<QDialog::DialogCode>(r);

	hide();
}

void Dialog::hideEvent(QHideEvent* event)
{
	emit dialogClosed(result());

	QFrame::hideEvent(event);
}

int Dialog::result() const
{
	return m_dialogCode;
}

}
