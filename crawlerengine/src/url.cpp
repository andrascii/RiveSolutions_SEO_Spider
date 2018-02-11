#include "url.h"

namespace CrawlerEngine
{

Url::Url(const QUrl& url)
	: QUrl(url)
{
}

const QString& Url::canonizedUrlStr() const
{
	if (m_canonizedUrlStr.isEmpty())
	{
		m_canonizedUrlStr = toDisplayString();
		if (m_canonizedUrlStr.endsWith(QString("/")))
		{
			m_canonizedUrlStr = m_canonizedUrlStr.left(m_canonizedUrlStr.size() - 1);
		}
	}

	return m_canonizedUrlStr;
}

}