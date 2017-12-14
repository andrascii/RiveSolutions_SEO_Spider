#include "custom_url.h"

namespace CrawlerEngine
{

bool CustomUrl::compareWith(const QUrl& url) const
{
	QString&& thisPath = path();
	QString&& urlPath = url.path();

	if (thisPath.endsWith("/"))
	{
		thisPath = thisPath.left(thisPath.size() - 1);
	}

	if (urlPath.endsWith("/"))
	{
		urlPath = urlPath.left(urlPath.size() - 1);
	}

	return scheme() == url.scheme() &&
		userName() == url.userName() &&
		password() == url.password() &&
		host() == url.host() &&
		port() == url.port() &&
		thisPath == urlPath &&
		query() == url.query() &&
		fragment() == url.fragment();
}

}