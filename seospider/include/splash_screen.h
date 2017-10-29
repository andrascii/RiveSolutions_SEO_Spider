#pragma once

#include "ui_splash_screen_widget.h"

namespace SeoSpider
{

class SplashScreen : public QWidget
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

	using QWidget::show;

	void setMessage(const QString& message) const;

private:
	static SplashScreen* s_instance;

	Ui_SplashScreenWidget* m_ui;
	QPixmap m_brandingLogoImage;
};

}
