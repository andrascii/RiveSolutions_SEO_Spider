#pragma once
#include "stdafx.h"

namespace SeoSpider
{

class FloatingFrame : public QFrame
{
	Q_OBJECT

public:
	explicit FloatingFrame(QWidget* parent = nullptr);

protected:
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void paintEvent(QPaintEvent* event);

private:
	bool m_mousePressed;
	QPoint m_mousePos;
	QPoint m_wndPos;
};

}