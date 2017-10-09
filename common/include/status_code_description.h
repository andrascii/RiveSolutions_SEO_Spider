#pragma once

#include "status_code.h"

namespace Common
{

class StatusCodeDescription
{
public:
	static const QString& description(StatusCode code);
};

}