#include "page_parser_helpers.h"

namespace CrawlerEngineTests
{

TEST(PageParserHelpersTests, IsHttpOrHttpsScheme)
{
	EXPECT_EQ(true, CrawlerEngine::PageParserHelpers::isHttpOrHttpsScheme(Url("index.html"))); // relative url
	EXPECT_EQ(true, CrawlerEngine::PageParserHelpers::isHttpOrHttpsScheme(Url("somewhere/index.html"))); // relative url
	EXPECT_EQ(true, CrawlerEngine::PageParserHelpers::isHttpOrHttpsScheme(Url("/somewhere/index.html"))); // relative url
	EXPECT_EQ(true, CrawlerEngine::PageParserHelpers::isHttpOrHttpsScheme(Url("127.0.0.1"))); // ip

	EXPECT_EQ(true, CrawlerEngine::PageParserHelpers::isHttpOrHttpsScheme(Url("http://somewhere.com/index.html"))); // http
	EXPECT_EQ(true, CrawlerEngine::PageParserHelpers::isHttpOrHttpsScheme(Url("https://somewhere.com/index.html"))); // https

	EXPECT_EQ(false, CrawlerEngine::PageParserHelpers::isHttpOrHttpsScheme(Url("httpt://somewhere.com/index.html"))); // httpt
	EXPECT_EQ(false, CrawlerEngine::PageParserHelpers::isHttpOrHttpsScheme(Url("httpst://somewhere.com/index.html"))); // httpst
	EXPECT_EQ(false, CrawlerEngine::PageParserHelpers::isHttpOrHttpsScheme(Url("ftp://somewhere.com/index.html"))); // ftp
	EXPECT_EQ(false, CrawlerEngine::PageParserHelpers::isHttpOrHttpsScheme(Url("mailto:nobody@nowhere.com"))); // mailto
	EXPECT_EQ(false, CrawlerEngine::PageParserHelpers::isHttpOrHttpsScheme(Url("data:image/gif;base64"))); // data:
	EXPECT_EQ(false, CrawlerEngine::PageParserHelpers::isHttpOrHttpsScheme(Url("skype:@vasya"))); // skype:
}

TEST(PageParserHelpersTests, IsSubDomain)
{
	EXPECT_EQ(true, PageParserHelpers::isSubdomain(Url("http://links.com/folder1/checkcss.html"), Url("http://domain.links.com/css.css")));
	EXPECT_EQ(true, PageParserHelpers::isSubdomain(Url("http://domain.links.com/css.css"), Url("http://links.com/folder1/checkcss.html")));

	// ports
	EXPECT_EQ(true, PageParserHelpers::isSubdomain(Url("http://links.com:80/folder1/checkcss.html"), Url("http://domain.links.com:81/css.css")));
	EXPECT_EQ(true, PageParserHelpers::isSubdomain(Url("http://domain.links.com:81/css.css"), Url("http://links.com/folder1:80/checkcss.html")));

	// www
	EXPECT_EQ(true, PageParserHelpers::isSubdomain(Url("http://www.links.com"), Url("http://links.com")));
	EXPECT_EQ(true, PageParserHelpers::isSubdomain(Url("http://links.com"), Url("http://www.links.com")));

	// different subdomains? should we treat these as subdomains?
	EXPECT_EQ(false, PageParserHelpers::isSubdomain(Url("http://a.links.com"), Url("http://b.links.com")));
	EXPECT_EQ(false, PageParserHelpers::isSubdomain(Url("http://b.links.com"), Url("http://a.links.com")));

	// not absolute urls
	EXPECT_EQ(false, PageParserHelpers::isSubdomain(Url("http://a.links.com"), Url("links.com")));
	EXPECT_EQ(false, PageParserHelpers::isSubdomain(Url("links.com"), Url("http://a.links.com")));

	EXPECT_EQ(false, PageParserHelpers::isSubdomain(Url("/index2.html"), Url("/index.html")));
	EXPECT_EQ(false, PageParserHelpers::isSubdomain(Url("/index.html"), Url("/index.html")));

	// not http schemas
	EXPECT_EQ(true, PageParserHelpers::isSubdomain(Url("http://links.com"), Url("https://b.links.com")));
	EXPECT_EQ(true, PageParserHelpers::isSubdomain(Url("https://b.links.com"), Url("http://links.com")));

	EXPECT_EQ(false, PageParserHelpers::isSubdomain(Url("ftp://links.com"), Url("https://b.links.com")));
	EXPECT_EQ(false, PageParserHelpers::isSubdomain(Url("https://b.links.com"), Url("ftp://links.com")));

	// the same domain
	EXPECT_EQ(false, PageParserHelpers::isSubdomain(Url("http://links.com"), Url("https://links.com")));
	EXPECT_EQ(false, PageParserHelpers::isSubdomain(Url("https://links.com"), Url("http://links.com")));

	EXPECT_EQ(false, PageParserHelpers::isSubdomain(Url("http://links.com/a"), Url("http://links.com")));
	EXPECT_EQ(false, PageParserHelpers::isSubdomain(Url("http://links.com"), Url("http://links.com/a")));

	EXPECT_EQ(false, PageParserHelpers::isSubdomain(Url("http://links.com:80"), Url("https://links.com:81")));
	EXPECT_EQ(false, PageParserHelpers::isSubdomain(Url("https://links.com:80"), Url("http://links.com:81")));


}

}
