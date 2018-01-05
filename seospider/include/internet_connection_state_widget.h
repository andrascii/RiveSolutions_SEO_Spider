#pragma once

namespace SeoSpider
{

class InternetConnectionStateWidget : public QWidget
{
public:
	InternetConnectionStateWidget(QWidget* parent = nullptr);

private slots:
	void onInternetConnectionStateChanged();

private:
	QPixmap internetConnectionStatePixmap() const;
	QString internetConnectionStateToolTip() const;

private:
	QLabel* m_pixmapLabel;
};

}