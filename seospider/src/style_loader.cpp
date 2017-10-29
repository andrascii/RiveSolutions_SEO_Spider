#include "style_loader.h"
#include "application.h"

namespace SeoSpider
{

void StyleLoader::attach(QString const& filename, QKeySequence const& keySequenceCustomStyleSheet)
{
	StyleLoader* styleLoader = new StyleLoader(theApp, filename, keySequenceCustomStyleSheet);
	theApp->installEventFilter(styleLoader);
}

bool StyleLoader::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

		if (m_keySequenceCustomStyleSheet == QKeySequence(keyEvent->key()))
		{
			if (keyEvent->modifiers() & Qt::ControlModifier)
			{
				loadStandardStyleSheet();
			}
			else
			{
				loadCustomStyleSheet();
			}

			return true;
		}
	}

	return QObject::eventFilter(obj, event);
}

StyleLoader::StyleLoader(QObject* parent, QString const& filename, QKeySequence const& keySequenceCustomStyleSheet)
	: QObject(parent)
	, m_filename(filename)
	, m_keySequenceCustomStyleSheet(keySequenceCustomStyleSheet)
{
}

void StyleLoader::loadCustomStyleSheet() const
{
	const QString filenamePath = "C:/" + m_filename;

	QFile styles(filenamePath);

	if (styles.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		const QString styleSheet = styles.readAll();
		qApp->setStyleSheet(styleSheet);

		DEBUGLOG << "StyleLoader" << QString("Debug styles loaded from %1").arg(filenamePath);
	}
	else
	{
		DEBUGLOG << "StyleLoader" << QString("Debug styles cannot be loaded from %1").arg(filenamePath);
	}
}

void StyleLoader::loadStandardStyleSheet() const
{
	DEBUGLOG << "StyleLoader" << QString("Loaded standard stylesheets");
	theApp->initializeStyleSheet();
}

}