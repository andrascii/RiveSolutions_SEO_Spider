#pragma once

namespace SeoSpider
{

class MenuBar : public QMenuBar
{
	Q_OBJECT

public:
	MenuBar(QWidget* parent = nullptr);

private:
	void init();

	QMenu* buildMenuFile();
	QMenu* buildMenuSettings();
	QMenu* buildMenuSitemap();
};

}