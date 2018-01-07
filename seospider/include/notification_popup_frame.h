#pragma once

namespace SeoSpider
{

class NotificationPopupFrame : public QFrame
{
	Q_OBJECT

public:
	enum Status
	{
		Info,
		Warning,
		Error
	};

	NotificationPopupFrame(Status status, const QString& header, const QString& message, QWidget* relativePosition);

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void showEvent(QShowEvent* event) override;

private:
	QWidget* createStatusPixmapWidget(Status status);

private slots:
	void setPosition();

private:
	QRect m_parentRect;
	const int m_borderWidth;
	const int m_angleWidth;
	const int m_angleHeight;

	QPixmap m_pixmap;
};

}