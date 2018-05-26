#pragma once

#include "ui_splash_screen_frame.h"

namespace SeoSpider
{

class SplashScreen : public QFrame
{
	Q_OBJECT

public:
	static void show();
	static void finish();
	static void showMessage(const QString& message);

protected:
	virtual void paintEvent(QPaintEvent* event) override;

private:
	SplashScreen();
	~SplashScreen();

	static SplashScreen* instance();

	void setMessage(const QString& message) const;

private:
	static QPointer<SplashScreen> s_instance;

	Ui_SplashScreenFrame* m_ui;
	QPixmap m_brandingLogoImage;
};

}
