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
};

}