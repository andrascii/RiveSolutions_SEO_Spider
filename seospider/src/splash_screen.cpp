#include "application.h"
#include "software_branding.h"
#include "splash_screen.h"
#include "helpers.h"

namespace SeoSpider
{

SplashScreen* SplashScreen::s_instance = nullptr;

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

void SplashScreen::show()
{
	instance()->QWidget::show();

	theApp->processEvents();
}

void SplashScreen::finish()
{
	instance()->hide();
	instance()->deleteLater();

	theApp->processEvents();
}

void SplashScreen::showMessage(const QString& message)
{
	instance()->setMessage(message);

	theApp->processEvents();
}

SplashScreen::SplashScreen()
	: QWidget(nullptr, Qt::Dialog | Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
	, m_ui(new Ui::SplashScreenWidget)
{
	m_brandingLogoImage = theApp->softwareBrandingOptions()->brandingLogoImage();

	m_ui->setupUi(this);

	const QSize originalImageSize = m_brandingLogoImage.size();

	resize(originalImageSize);

#ifdef PRODUCTION
	QString version = "13.13";
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

	QPen pen(QColor("#D6D6D6"));
	pen.setWidth(Common::Helpers::pointsToPixels(2));

	painter.setPen(pen);

	const QRect adjustedRect = rect().adjusted(0, 0, -1, -1);

	painter.drawPixmap(rect(), m_brandingLogoImage);
	painter.drawRect(adjustedRect);
}

}