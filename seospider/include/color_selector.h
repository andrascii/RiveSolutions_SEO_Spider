#pragma once
#include "stdafx.h"

namespace SeoSpider
{
	
class ColorSelector : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)

public:
	ColorSelector(QWidget* parent = nullptr);

	QColor getColor() const;

	const QColor& borderColor() const noexcept;
	Q_SLOT void setBorderColor(const QColor& color) noexcept;
	Q_SIGNAL void borderColorChanged();

public slots:
	void setColor(QColor value);

signals:
	void colorChanged();

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent*) override;

private:
	const int m_borderWidth;
	QColor m_borderColor;

	QColor m_color;
};

}