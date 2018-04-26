#pragma once

namespace SeoSpider
{

#ifndef PRODUCTION
const unsigned int c_optimalParserThreadsCount = std::thread::hardware_concurrency() / 2;
#else
const unsigned int c_optimalParserThreadsCount = std::thread::hardware_concurrency();
#endif

const QByteArray c_projectFileExtension = ".ssproj";

}