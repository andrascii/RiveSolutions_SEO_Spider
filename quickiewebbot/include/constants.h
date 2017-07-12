#pragma once

namespace QuickieWebBot
{

const int g_minimumRecommendedTimerResolution = 21;
const unsigned int g_optimalParserThreadsCount = std::thread::hardware_concurrency() / 2;

}