#include "floating_dialog.h"
#include "dialog.h"
#include "application.h"
#include "main_window.h"
#include "dialog_container.h"

namespace SeoSpider
{

FloatingDialog::FloatingDialog(QWidget* parent)
	: FloatingFrame(parent)
{
}

QObject* FloatingDialog::qobject() const
{
	return const_cast<FloatingDialog*>(this);
}

void FloatingDialog::accept()
{
	done(QDialog::Accepted);
}

void FloatingDialog::reject()
{
	done(QDialog::Rejected);
}

void FloatingDialog::open()
{
	theApp->mainWindow()->showShadedOverlay();

	DialogContainer::instance().openDialog(this);

	theApp->mainWindow()->hideShadedOverlay();
}

void FloatingDialog::done(int r)
{
	m_dialogCode = static_cast<QDialog::DialogCode>(r);

	hide();
}

int FloatingDialog::result() const
{
	return m_dialogCode;
}

void FloatingDialog::hideEvent(QHideEvent* event)
{
	emit dialogClosed(result());

	QFrame::hideEvent(event);
}

}
