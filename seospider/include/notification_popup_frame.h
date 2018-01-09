#pragma once

namespace SeoSpider
{

class NotificationPopupFrame : public QFrame
{
	Q_OBJECT

	Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)
	Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)

public:
	enum Status
	{
		Info,
		Warning,
		Error
	};

	NotificationPopupFrame(Status status, const QString& header, const QString& message, QWidget* relativePosition);

	const QColor& borderColor() const noexcept;
	Q_SLOT void setBorderColor(const QColor& color) noexcept;
	Q_SIGNAL void borderColorChanged();

	const QColor& backgroundColor() const noexcept;
	Q_SLOT void setBackgroundColor(const QColor& color) noexcept;
	Q_SIGNAL void backgroundColorChanged();

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void showEvent(QShowEvent* event) override;

private:
	QWidget* createStatusPixmapWidget(Status status);

private slots:
	void setPosition();
	void redrawPixmap();

private:
	const int m_borderWidth;
	const int m_angleWidth;
	const int m_angleHeight;

	QPixmap m_pixmap;

	QColor m_borderColor;
	QColor m_backgroundColor;
};

}