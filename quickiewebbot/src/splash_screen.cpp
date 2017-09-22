#include "application.h"
#include "software_branding.h"
#include "splash_screen.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

SplashScreen* SplashScreen::s_instance = nullptr;
bool SplashScreen::s_alreadyShown = false;


SplashScreen* SplashScreen::instance()
{
	if (!s_instance)
	{
		s_instance = new SplashScreen;
	}

	return s_instance;
}

void SplashScreen::show()
{
	m_timer->setInterval(3000);
	m_timer->setSingleShot(true);

	VERIFY(connect(m_timer, &QTimer::timeout, this, &QWidget::deleteLater));

	QWidget::show();

	m_timer->start();
}

SplashScreen::SplashScreen()
	: QWidget(nullptr, Qt::Dialog | Qt::Tool | Qt::FramelessWindowHint)
	, m_screenContent(new Ui::SplashScreenWidget)
	, m_timer(new QTimer(this))
{
	DEBUG_ASSERT(!s_alreadyShown);

	s_alreadyShown = true;

	m_brandingLogoImage = theApp->softwareBrandingOptions()->brandingLogoImage();

	m_screenContent->setupUi(this);

	// Temporary hide
	m_screenContent->versionText->hide();
	m_screenContent->copyrightText->hide();
	m_screenContent->label->hide();
	m_screenContent->label_2->hide();

	resize(m_brandingLogoImage.size());
}

SplashScreen::~SplashScreen()
{
	emit finished();

	s_instance = nullptr;
}

void SplashScreen::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);

	QPainter painter(this);

	QPen pen(QColor("#D6D6D6"));
	pen.setWidth(QuickieWebBotHelpers::pointsToPixels(2));

	painter.setPen(pen);

	QRect adjustedRect = rect().adjusted(0, 0, -1, -1);

	painter.drawPixmap(rect(), m_brandingLogoImage);
	painter.drawRect(adjustedRect);
}

}