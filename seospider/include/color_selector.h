#pragma once

namespace SeoSpider
{
	
class ColorSelector : public QObject
{
	Q_OBJECT
public:
	ColorSelector(QObject* parent = nullptr);

	QColor getColor() const;
	void setColor(QColor value);

signals:
	void colorChanged();

private:
	QColor m_color;
};

}