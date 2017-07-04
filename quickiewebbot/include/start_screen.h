#pragma once

#include "ui_start_screen.h"

namespace QuickieWebBot
{

class StartScreen : public QWidget
{
	Q_OBJECT

public:
	static StartScreen* instance();

	void show();

	Q_SIGNAL void finished();

protected:
	virtual void paintEvent(QPaintEvent* event) override;

private:
	StartScreen();
	~StartScreen();

private:
	static StartScreen* s_instance;
	static bool s_alreadyShown;

	std::unique_ptr<Ui::StartScreen> m_startScreenContent;

	QPixmap m_brandingLogoImage;
	QTimer* m_timer;
};

}
