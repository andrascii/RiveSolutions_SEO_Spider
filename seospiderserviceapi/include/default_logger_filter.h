#pragma once

#include "ilogger_filter.h"

namespace SeoSpiderServiceApi
{

class DefaultLoggerFilter : public ILoggerFilter
{
public:
    virtual bool operator()(SeverityLevel level) const noexcept override
    {
#ifdef PRODUCTION
        SeverityLevel startLevel = SeverityLevel::InfoLevel;
#else
        SeverityLevel startLevel = SeverityLevel::TraceLevel;
#endif
        return level >= startLevel;
    }
};

}