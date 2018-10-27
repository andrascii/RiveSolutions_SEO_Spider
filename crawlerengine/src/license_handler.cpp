#include "license_handler.h"
#include "handler_registry.h"
#include "helpers.h"
#include "my_license_service.h"
#include "vm_protect_license_service.h"
#include "thread_message_dispatcher.h"
#include "set_serial_number_request.h"
#include "set_serial_number_response.h"
#include "get_serial_number_data_request.h"
#include "get_serial_number_data_response.h"
#include "get_serial_number_state_request.h"
#include "get_serial_number_state_response.h"

namespace
{

constexpr int c_myServiceRawDataParts = 3;

}

namespace CrawlerEngine
{

LicenseHandler::LicenseHandler()
	: m_licenseService(nullptr)
{
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();

	handlerRegistry.registrateHandler(this,
		RequestType::RequestSetSerialNumber,
		RequestType::RequestGetSerialNumberData,
		RequestType::RequestGetSerialNumberState
	);
}

LicenseHandler::~LicenseHandler()
{
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.unregistrateHandler(this);
}

void LicenseHandler::handleRequest(RequesterSharedPtr requester)
{
	switch (requester->request()->requestType())
	{
		case RequestType::RequestSetSerialNumber:
		{
			setSerialNumber(requester);
			break;
		}
		case RequestType::RequestGetSerialNumberData:
		{
			getSerialNumberData(requester);
			break;
		}
		case RequestType::RequestGetSerialNumberState:
		{
			getSerialNumberState(requester);
			break;
		}
		default:
		{
			DEBUG_ASSERT(!"Unknown request type");
		}
	}
}

void LicenseHandler::stopRequestHandling(RequesterSharedPtr)
{
}

void LicenseHandler::setSerialNumber(const RequesterSharedPtr& requester)
{
	SetSerialNumberRequest* request = Common::Helpers::fast_cast<SetSerialNumberRequest*>(requester->request());

	std::shared_ptr<SetSerialNumberResponse> response =
		std::make_shared<SetSerialNumberResponse>(setSerialNumberInternal(request->serialNumber()));

	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, response);
}

void LicenseHandler::getSerialNumberData(const RequesterSharedPtr& requester)
{
	const SerialNumberData responseData = m_licenseService ? m_licenseService->serialNumberData() : SerialNumberData();

	std::shared_ptr<GetSerialNumberDataResponse> response =
		std::make_shared<GetSerialNumberDataResponse>(responseData);

	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, response);
}

void LicenseHandler::getSerialNumberState(const RequesterSharedPtr& requester)
{
	SerialNumberStates responseData;

	if (m_licenseService)
	{
		responseData = m_licenseService->serialNumberStates();
	}
	else
	{
		responseData.setFlag(SerialNumberState::StateInvalidSerialNumberActivation);
	}

	std::shared_ptr<GetSerialNumberStateResponse> response =
		std::make_shared<GetSerialNumberStateResponse>(responseData);

	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, response);
}

void LicenseHandler::initLicenseServiceByKey(const QByteArray& key)
{
	if (m_licenseService)
	{
		QObject* licenseServiceObject = dynamic_cast<QObject*>(m_licenseService);

		ASSERT(licenseServiceObject);

		licenseServiceObject->deleteLater();
	}

	m_licenseService = isMyLicenseServiceKeyType(key) ?
		static_cast<ILicenseService*>(new MyLicenseService(this)) :
		static_cast<ILicenseService*>(new VmProtectLicenseService(this));
}

bool LicenseHandler::isMyLicenseServiceKeyType(const QByteArray& key) const
{
	const QByteArray& rawData = Common::Helpers::decryptAesKey(key, QByteArray("111111111111111111111111"));

	QList<QByteArray> rawDataParts = rawData.split(' ');

	if (rawDataParts.size() != c_myServiceRawDataParts)
	{
		return false;
	}

	QDate date = QDate::fromString(rawDataParts[2]);

	return !date.isNull();
}

SerialNumberStates LicenseHandler::setSerialNumberInternal(const QByteArray& serialNumber)
{
	initLicenseServiceByKey(serialNumber);
	return m_licenseService->setSerialNumber(serialNumber);
}

}