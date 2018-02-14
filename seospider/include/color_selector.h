#pragma once

namespace SeoSpider
{
	
class ColorSelector : public QWidget
{
	Q_OBJECT
public:
	ColorSelector(QWidget* parent = nullptr);

	QColor getColor() const;

public slots:
	void setColor(QColor value);

signals:
	void colorChanged();

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent*) override;

private:
	QColor m_borderColor;
	QColor m_color;
};

}