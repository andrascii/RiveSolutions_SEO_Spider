#pragma once

namespace Common
{

const int g_minimumRecommendedTimerResolution = 21;

#ifndef PRODUCTION
const unsigned int g_optimalParserThreadsCount = std::thread::hardware_concurrency() / 2;
#else
const unsigned int g_optimalParserThreadsCount = std::thread::hardware_concurrency();
#endif

}