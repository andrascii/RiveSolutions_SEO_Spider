#pragma once

namespace WebCrawler
{
	
class ResourcesCache
{
public:
	void clear();
	bool isResourceExists(const QUrl& url) const;

private:
	mutable std::set<QString> m_resources;
};

}