#pragma once

namespace Common
{

const int g_minimumRecommendedTimerResolution = 21;
const unsigned int g_optimalParserThreadsCount = std::thread::hardware_concurrency();

}