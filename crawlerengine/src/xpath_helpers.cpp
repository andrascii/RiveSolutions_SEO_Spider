#include "xpath_helpers.h"

namespace CrawlerEngine
{
	
QString XPathHelpers::evaluateXPath(const QString& xml, const QString& query, const QString& defaultNamespace)
{
	QXmlQuery xmlQuery(QXmlQuery::XQuery10);
	xmlQuery.setFocus(xml);

	QString fullQuery;
	if (!defaultNamespace.isEmpty())
	{
		fullQuery = fullQuery % QString("declare default element namespace \"") % defaultNamespace % QString("\"; ");
	}

	fullQuery = fullQuery % query;
	xmlQuery.setQuery(fullQuery);

	DEBUG_ASSERT(query.isValid());

	QString result;
	xmlQuery.evaluateTo(&result);

	return result.trimmed();
}

}