#include "widget_detector.h"

namespace QuickieWebBot
{

void WidgetDetector::attachWidgetDetector(QKeySequence const& keySequence)
{
	WidgetDetector* widgetDetector = new WidgetDetector(qApp, keySequence);
	qApp->installEventFilter(widgetDetector);
}

bool WidgetDetector::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* actualEvent = static_cast<QKeyEvent*>(event);

		if (m_keySequence == QKeySequence(actualEvent->key()))
		{
			showMessageBoxInfo();

			return true;
		}
	}

	return QObject::eventFilter(obj, event);
}

WidgetDetector::WidgetDetector(QObject* parent, QKeySequence const& keySequence)
	: QObject(parent)
	, m_keySequence(keySequence)
{
}

void WidgetDetector::showMessageBoxInfo()
{
	QWidget* widgetUnderCursor = qApp->widgetAt(QPoint(QCursor::pos()));

	if (widgetUnderCursor)
	{
		QString objectInfo = QStringLiteral("Object name: %1\nApplied stylesheets: %2")
			.arg(widgetUnderCursor->objectName())
			.arg(widgetUnderCursor->styleSheet());

		QMessageBox::information(nullptr, "Widget info", objectInfo);
	}
}

}