#pragma once

namespace CrawlerEngine
{

enum class ExtractionType
{
    TypeExtractInnerHtml,
    TypeExtractHtmlElement,
    TypeExtractText,
    TypeExtractFunctionValue
};

}

Q_DECLARE_METATYPE(CrawlerEngine::ExtractionType);