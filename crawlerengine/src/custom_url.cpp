#include "custom_url.h"

namespace CrawlerEngine
{

bool CustomUrl::compareWith(const CustomUrl& url) const
{
	return canonizedUrlStr() == url.canonizedUrlStr();
}

const QString& CustomUrl::canonizedUrlStr() const
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