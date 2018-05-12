#include "license_handler.h"
#include "handler_registry.h"
#include "helpers.h"
#include "thread_message_dispatcher.h"
#include "set_serial_number_request.h"
#include "set_serial_number_response.h"
#include "get_serial_number_data_request.h"
#include "get_serial_number_data_response.h"
#include "get_serial_number_state_request.h"
#include "get_serial_number_state_response.h"

namespace CrawlerEngine
{

LicenseHandler::LicenseHandler()
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
		std::make_shared<SetSerialNumberResponse>(QFlag(VMProtectSetSerialNumber(request->serialNumber().data())));

	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, response);
}

void LicenseHandler::getSerialNumberData(const RequesterSharedPtr& requester)
{
	VMProtectSerialNumberData data{0};
	VMProtectGetSerialNumberData(&data, sizeof(data));

	std::shared_ptr<GetSerialNumberDataResponse> response =
		std::make_shared<GetSerialNumberDataResponse>(data);

	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, response);
}

void LicenseHandler::getSerialNumberState(const RequesterSharedPtr& requester)
{
	std::shared_ptr<GetSerialNumberStateResponse> response =
		std::make_shared<GetSerialNumberStateResponse>(QFlag(VMProtectGetSerialNumberState()));

	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, response);
}

}