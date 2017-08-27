#pragma once

namespace QuickieWebBot
{

class MainFrameMenuBar : public QMenuBar
{
	Q_OBJECT

public:
	MainFrameMenuBar(QWidget* parent = nullptr);

private:
	void init();

	QMenu* buildMenuFile();
	QMenu* buildMenuSettings();
};

}