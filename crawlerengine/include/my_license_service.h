#pragma once

#include "abstract_license_service.h"
#include "serial_number_data.h"
#include "requester_wrapper.h"

namespace CrawlerEngine
{

struct DownloadResponse;

class MyLicenseService : public QObject, public AbstractLicenseService
{
	Q_OBJECT

public:
	MyLicenseService(QObject* parent = nullptr);

	virtual SerialNumberStates setSerialNumber(const QByteArray& serialNumber) override;
	virtual SerialNumberData serialNumberData() const override;
	virtual SerialNumberStates serialNumberStates() const override;

	void requestSerialNumberData(const RequesterSharedPtr& requester);

private:
	virtual void timerEvent(QTimerEvent* event) override;
	virtual void verifyKey();

	void onLoadingDone(Requester* requester, const DownloadResponse& response);
	void respondSerialNumberData();

private:
	SerialNumberData m_data;
	QByteArray m_validSerialNumber;
	RequesterWrapper m_verifyKeyRequester;
	QList<RequesterWeakPtr> m_requesters;
};

}