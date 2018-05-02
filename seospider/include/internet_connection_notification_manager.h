#pragma once

namespace SeoSpider
{

class InternetConnectionNotificationManager : public QObject
{
	Q_OBJECT

public:
	InternetConnectionNotificationManager(QObject* parent);
	~InternetConnectionNotificationManager();

	bool internetAvailable() const noexcept;

signals:
	void internetConnectionStateChanged();

private slots:
	void onAboutInternetConnectionStateChanged(bool value);

private:
	static InternetConnectionNotificationManager* s_selfPointer;

	bool m_internetAvailable;
};

}