#pragma once
#include "stdafx.h"

namespace SeoSpider
{

class CollapseHeaderButton : public QToolButton
{
	Q_OBJECT

	Q_PROPERTY(int rotateDegree READ rotateDegree WRITE setRotateDegree NOTIFY rotateDegreeChanged)

public:
	enum ArrowDirection
	{
		ArrowDirectionUp,
		ArrowDirectionDown
	};

	CollapseHeaderButton(QWidget* parent = nullptr);
	CollapseHeaderButton(ArrowDirection direction, QWidget* parent = nullptr);

	int rotateDegree() const noexcept;

	void setArrowDirection(ArrowDirection direction);
	ArrowDirection arrowDirection() const noexcept;

signals:
	void rotateDegreeChanged();

private slots:
	void setRotateDegree(int degree);

private:
	int rotateDegreeByDirection(ArrowDirection direction);

private:
	int m_rotateDegree;
	ArrowDirection m_direction;
	QPixmap m_pixmap;
};

}