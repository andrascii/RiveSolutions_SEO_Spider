#include "application.h"
#include "software_branding.h"
#include "start_screen.h"

namespace QuickieWebBot
{

StartScreen* StartScreen::s_instance = nullptr;
bool StartScreen::s_alreadyShown = false;


StartScreen* StartScreen::instance()
{
	if (!s_instance)
	{
		s_instance = new StartScreen;
	}

	return s_instance;
}

void StartScreen::show()
{
	m_timer->setInterval(3000);
	m_timer->setSingleShot(true);

	VERIFY(connect(m_timer, &QTimer::timeout, this, &QWidget::deleteLater));

	QWidget::show();

	m_timer->start();
}

StartScreen::StartScreen()
	: QWidget(nullptr, Qt::Dialog | Qt::Tool | Qt::FramelessWindowHint)
	, m_startScreenContent(new Ui::StartScreen)
	, m_timer(new QTimer(this))
{
	assert(!s_alreadyShown);

	s_alreadyShown = true;

	m_brandingLogoImage = theApp->softwareBrandingOptions()->brandingLogoImage();

	m_startScreenContent->setupUi(this);

	// Temporary hide
	m_startScreenContent->versionText->hide();
	m_startScreenContent->copyrightText->hide();
	m_startScreenContent->label->hide();
	m_startScreenContent->label_2->hide();

	resize(m_brandingLogoImage.size());
}

StartScreen::~StartScreen()
{
	emit finished();

	s_instance = nullptr;
}

void StartScreen::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);

	QPainter painter(this);
	painter.drawPixmap(rect(), m_brandingLogoImage);
}

}