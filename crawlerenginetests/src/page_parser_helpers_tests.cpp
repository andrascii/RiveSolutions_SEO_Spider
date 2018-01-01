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

}
