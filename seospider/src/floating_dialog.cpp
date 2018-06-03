#include "floating_dialog.h"
#include "dialog.h"

namespace SeoSpider
{

FloatingDialog::FloatingDialog(QWidget* parent)
	: FloatingFrame(parent)
	, m_dialog(new Dialog(this))
{

}

void FloatingDialog::accept()
{
	m_dialog->accept();
}

void FloatingDialog::reject()
{
	m_dialog->reject();
}

void FloatingDialog::open()
{
	m_dialog->open();
}

void FloatingDialog::done(int r)
{
	m_dialog->done(r);
}

int FloatingDialog::result() const
{
	return m_dialog->result();
}

bool FloatingDialog::eventFilter(QObject*, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress)
	{
		FloatingFrame::mousePressEvent(static_cast<QMouseEvent*>(event));
	}

	if (event->type() == QEvent::MouseButtonRelease)
	{
		FloatingFrame::mouseReleaseEvent(static_cast<QMouseEvent*>(event));
	}

	if (event->type() == QEvent::MouseMove)
	{
		FloatingFrame::mouseMoveEvent(static_cast<QMouseEvent*>(event));
	}

	return false;
}

}
