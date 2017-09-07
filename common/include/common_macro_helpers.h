#pragma once

#include "debug_helpers_api.h"

#define ASSERT(condition) if((condition) == false) Common::DebugHelpersApi::doAssert(__FILE__, __LINE__, __FUNCTION__, #condition);

#ifdef QT_DEBUG
#define DEBUG_ASSERT(condition) ASSERT(condition)
#else
#define DEBUG_ASSERT(condition)
#endif

#ifdef QT_DEBUG
#define VERIFY(Connection) ASSERT(Connection)
#else
#define VERIFY(Connection) Connection
#endif