#pragma once

#include "ilogger.h"

namespace SeoSpiderServiceApi
{

class ILoggerFilter
{
public:
    virtual bool operator()(SeverityLevel level) const noexcept = 0;
};

}