#include "widget_under_mouse_info.h"
#include "widget_under_mouse_info_dialog.h"
#include "application.h"

namespace QuickieWebBot
{

void WidgetUnderMouseInfo::attach(QKeySequence const& keySequence)
{
	WidgetUnderMouseInfo* styleLoader = new WidgetUnderMouseInfo(theApp, keySequence);
	theApp->installEventFilter(styleLoader);
}

bool WidgetUnderMouseInfo::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

		if (m_keySequence == QKeySequence(keyEvent->key()))
		{
			QWidget* widgetUnderMouse = theApp->widgetAt(QCursor::pos());
			const QMetaObject* metaObject = widgetUnderMouse->metaObject();

			m_widgetUnderMouseInfoDialog->showWithInfo(widgetUnderMouse->objectName(), metaObject->className());

			return true;
		}
	}

	return QObject::eventFilter(watched, event);
}

WidgetUnderMouseInfo::WidgetUnderMouseInfo(QObject* parent, QKeySequence const& keySequence)
	: QObject(parent)
	, m_keySequence(keySequence)
	, m_widgetUnderMouseInfoDialog(new WidgetUnderMouseInfoDialog(theApp->mainFrame()))
{
	VERIFY(connect(theApp, SIGNAL(aboutToQuit()), this, SLOT(deleteWidgetOnApplicationQuit())));
}

void WidgetUnderMouseInfo::deleteWidgetOnApplicationQuit()
{
	m_widgetUnderMouseInfoDialog.reset();
}

}