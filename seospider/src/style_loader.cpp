#include "style_loader.h"
#include "application.h"
#include "service_locator.h"
#include "inotification_service.h"

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

	CrawlerEngine::ServiceLocator* serviceLocator = CrawlerEngine::ServiceLocator::instance();
	CrawlerEngine::INotificationService* notificationService = serviceLocator->service<CrawlerEngine::INotificationService>();

	ASSERT(notificationService);

	if (styles.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		const QString styleSheet = styles.readAll();
		qApp->setStyleSheet(styleSheet);

		notificationService->info(QStringLiteral("StyleLoader"), QStringLiteral("Debug styles loaded from %1").arg(filenamePath));
	}
	else
	{
		notificationService->info(QStringLiteral("StyleLoader"), QStringLiteral("Debug styles cannot be loaded from %1").arg(filenamePath));
	}
}

void StyleLoader::loadStandardStyleSheet() const
{
	CrawlerEngine::ServiceLocator* serviceLocator = CrawlerEngine::ServiceLocator::instance();
	CrawlerEngine::INotificationService* notificationService = serviceLocator->service<CrawlerEngine::INotificationService>();

	ASSERT(notificationService);

	notificationService->info(QStringLiteral("StyleLoader"), QStringLiteral("Loaded standard stylesheets"));

	theApp->initializeStyleSheet();
}

}