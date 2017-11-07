#pragma once

namespace CrawlerEngine
{
	
class XPathHelpers
{
public:
	static QString evaluateXPath(const QString& xml, const QString& query, const QString& defaultNamespace = QString());


};

}