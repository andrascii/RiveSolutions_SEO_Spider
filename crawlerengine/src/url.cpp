#include "url.h"

namespace CrawlerEngine
{

Url::Url(const QUrl& url)
	: QUrl(url)
{
#ifndef PRODUCTION
	canonizedUrlStr();
#endif
}

const QString& Url::canonizedUrlStr() const
{
#ifdef PRODUCTION
	if (m_canonizedUrlStr.isEmpty())
	{
#endif
		m_canonizedUrlStr = toDisplayString();
		if (m_canonizedUrlStr.endsWith(QString("/")))
		{
			m_canonizedUrlStr = m_canonizedUrlStr.left(m_canonizedUrlStr.size() - 1);
		}
		if (m_canonizedUrlStr.startsWith(QString("http://www.")))
		{
			m_canonizedUrlStr = QString("http://") + m_canonizedUrlStr.mid(11);
		}
		else if (m_canonizedUrlStr.startsWith(QString("https://www.")))
		{
			m_canonizedUrlStr = QString("https://") + m_canonizedUrlStr.mid(12);
		}
#ifdef PRODUCTION
	}
#endif

	return m_canonizedUrlStr;
}

}