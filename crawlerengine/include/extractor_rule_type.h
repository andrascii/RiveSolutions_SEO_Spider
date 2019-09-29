#pragma once

namespace CrawlerEngine
{

enum class ExtractorRuleType
{
    TypeXPath,
    TypeCssPath,
    TypeRegex
};

}

Q_DECLARE_METATYPE(CrawlerEngine::ExtractorRuleType);