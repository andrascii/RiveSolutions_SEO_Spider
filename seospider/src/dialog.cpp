#include "dialog.h"
#include "shadow_decoration_frame.h"
#include "cursor_factory.h"
#include "widget_helpers.h"
#include "application.h"
#include "main_window.h"

namespace SeoSpider
{

Dialog::Dialog(QWidget* decoratedWidget, QWidget* parent)
	: QFrame(parent)
{
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	setWindowModality(Qt::ApplicationModal);
	setAttribute(Qt::WA_DeleteOnClose, true);
	
	QVBoxLayout* layout = new QVBoxLayout(parent);
	layout->addWidget(new ShadowDecorationFrame(decoratedWidget, parent));
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
	show();

	m_eventLoop.exec();

	completeLocalEventLoop();
}

void Dialog::done(int r)
{
	completeLocalEventLoop();

	m_dialogCode = static_cast<QDialog::DialogCode>(r);

	hide();
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
