#pragma once

namespace QuickieWebBot
{

class IHtmlParser
{
public:
	virtual void setupHtmlPage(const QString& htmlPage) = 0;

};

}