#pragma once

namespace SeoSpider
{

class NotificationPopupWidget : public QWidget
{
	Q_OBJECT

public:
	enum Status
	{
		Info,
		Warning,
		Error
	};

	NotificationPopupWidget(Status status, const QString& header, const QString& message, QWidget* relativePosition);

	void setStatus(Status status);
	void setHeader(const QString& header);
	void setMessage(const QString& message);

protected:
	virtual void paintEvent(QPaintEvent* event) override;

private:
	Status m_status;
	QString m_header;
	QString m_message;
	QRect m_parentRect;
};

}