#include "dialog.h"
#include "dialog_container.h"
#include "application.h"
#include "main_window.h"

namespace SeoSpider
{

Dialog::Dialog(QWidget* parent)
	: QFrame(parent)
{
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	setWindowModality(Qt::ApplicationModal);
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

	/*int result =*/ DialogContainer::instance().openDialog(this);

	//done(result);

// 	show();
// 
// 	m_eventLoop.exec();
// 
// 	completeLocalEventLoop();

	theApp->mainWindow()->hideShadedOverlay();
}

void Dialog::done(int r)
{
	completeLocalEventLoop();

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

void Dialog::completeLocalEventLoop()
{
	if (!m_eventLoop.isRunning())
	{
		return;
	}

	m_eventLoop.processEvents();
	m_eventLoop.exit();
}

}
