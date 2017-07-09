#include "style_loader.h"

namespace QuickieWebBot
{

void StyleLoader::attachStyleLoader(QString const& filename, QKeySequence const& keySequence)
{
	StyleLoader* styleLoader = new StyleLoader(qApp, filename, keySequence);
	qApp->installEventFilter(styleLoader);
}

bool StyleLoader::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

		if (m_keySequence == QKeySequence(keyEvent->key()))
		{
			updateStyleSheet();
		}
	}

	return QObject::eventFilter(obj, event);
}

StyleLoader::StyleLoader(QObject* parent, QString const& filename, QKeySequence const& keySequence)
	: QObject(parent)
	, m_filename(filename)
	, m_keySequence(keySequence)
{
}

void StyleLoader::updateStyleSheet()
{
	QFile styles("C:/" + m_filename);

	if (styles.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QString styleSheet = styles.readAll();
		qApp->setStyleSheet(styleSheet);
	}
}

}