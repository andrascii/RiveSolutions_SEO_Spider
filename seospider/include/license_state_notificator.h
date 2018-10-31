#pragma once

namespace SeoSpider
{

class LicenseStateNotificator final : public QObject
{
	Q_OBJECT

public:
	LicenseStateNotificator(QObject* parent);

private slots:
	void onLicenseStateChanged(int reason);
	void onDialogClosed();

private:
	std::vector<std::function<void()>> m_deferredNotifications;
};

}