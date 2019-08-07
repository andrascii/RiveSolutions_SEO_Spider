#pragma once

namespace SeoSpider
{

class ShadowDecorationFrame : public QFrame
{
	Q_OBJECT

public:
	ShadowDecorationFrame(QWidget* decoratingWidget, QWidget* parent = nullptr);

private:
	QHBoxLayout* m_bodyLayout;
	QWidget* m_decoratingWidget;
};

}