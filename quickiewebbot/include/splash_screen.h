#pragma once

#include "ui_splash_screen_widget.h"

namespace QuickieWebBot
{

class SplashScreen : public QWidget
{
	Q_OBJECT

public:
	static SplashScreen* instance();

	void show();

	Q_SIGNAL void finished();

protected:
	virtual void paintEvent(QPaintEvent* event) override;

private:
	SplashScreen();
	~SplashScreen();

private:
	static SplashScreen* s_instance;
	static bool s_alreadyShown;

	std::unique_ptr<Ui::SplashScreenWidget> m_screenContent;

	QPixmap m_brandingLogoImage;
	QTimer* m_timer;
};

}
