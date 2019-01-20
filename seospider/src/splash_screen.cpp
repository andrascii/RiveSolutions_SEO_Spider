#include "application.h"
#include "software_branding.h"
#include "splash_screen.h"
#include "helpers.h"
#include "version.h"

namespace SeoSpider
{

QPointer<SplashScreen> SplashScreen::s_instance = nullptr;

SplashScreen* SplashScreen::instance()
{
	if (!s_instance)
	{
		s_instance = new SplashScreen;
	}

	return s_instance;
}

void SplashScreen::setMessage(const QString& message) const
{
	m_ui->messageText->setText(message);
}

void SplashScreen::show(bool isAboutDialog)
{
	instance()->m_isAboutDialog = isAboutDialog;
	instance()->QWidget::show();

	if (isAboutDialog)
	{
		theApp->installEventFilter(instance());
	}

	theApp->processEvents();
}

void SplashScreen::finish()
{
	if (instance()->m_isAboutDialog)
	{
		theApp->removeEventFilter(instance());
	}

	instance()->hide();

	theApp->processEvents();
}

void SplashScreen::showMessage(const QString& message)
{
	instance()->setMessage(message);

	theApp->processEvents();
}

SplashScreen::SplashScreen()
	: QFrame(nullptr, Qt::Dialog | Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
	, m_ui(new Ui::SplashScreenFrame)
{
	m_brandingLogoImage = theApp->softwareBrandingOptions()->brandingLogoImage();

	m_ui->setupUi(this);

	const QSize originalImageSize = m_brandingLogoImage.size();

	resize(originalImageSize);

#ifdef PRODUCTION
	QString version = QString("%1.%2.%3").arg(MAJOR).arg(MINOR).arg(MAINTENANCE);
#else
	QString version = theApp->applicationVersion().isEmpty() ? "Developer's version" : theApp->applicationVersion();
#endif

	m_ui->versionText->setText(version);
}

SplashScreen::~SplashScreen()
{
	s_instance = nullptr;
}

void SplashScreen::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	const QRect adjustedRect = rect().adjusted(0, 0, -1, -1);
	painter.drawPixmap(rect(), m_brandingLogoImage);
}

bool SplashScreen::eventFilter(QObject* watched, QEvent* event)
{
	if (m_isAboutDialog && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::WindowDeactivate))
	{
		finish();
	}

	return QFrame::eventFilter(watched, event);
}
}
