#pragma once

namespace SeoSpider
{

class ShadedOverlay : public QWidget
{
public:
	ShadedOverlay(QWidget* parent);

protected:
	virtual void paintEvent(QPaintEvent* event)override;
};

}