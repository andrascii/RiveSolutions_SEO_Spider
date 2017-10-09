#pragma once

#include "response_code.h"

namespace Common
{

class ResponseCodeDescription
{
public:
	static const QString& description(ResponseCode code);
};

}