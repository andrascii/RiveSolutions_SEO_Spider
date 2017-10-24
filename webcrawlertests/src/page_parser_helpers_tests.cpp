#include "page_parser_helpers.h"

namespace WebCrawlerTests
{

TEST(PageParserHelpersTests, IsHttpOrHttpsScheme)
{
	EXPECT_EQ(true, WebCrawler::PageParserHelpers::isHttpOrHttpsScheme(QUrl("index.html"))); // relative url
	EXPECT_EQ(true, WebCrawler::PageParserHelpers::isHttpOrHttpsScheme(QUrl("somewhere/index.html"))); // relative url
	EXPECT_EQ(true, WebCrawler::PageParserHelpers::isHttpOrHttpsScheme(QUrl("/somewhere/index.html"))); // relative url
	EXPECT_EQ(true, WebCrawler::PageParserHelpers::isHttpOrHttpsScheme(QUrl("127.0.0.1"))); // ip

	EXPECT_EQ(true, WebCrawler::PageParserHelpers::isHttpOrHttpsScheme(QUrl("http://somewhere.com/index.html"))); // http
	EXPECT_EQ(true, WebCrawler::PageParserHelpers::isHttpOrHttpsScheme(QUrl("https://somewhere.com/index.html"))); // https

	EXPECT_EQ(false, WebCrawler::PageParserHelpers::isHttpOrHttpsScheme(QUrl("httpt://somewhere.com/index.html"))); // httpt
	EXPECT_EQ(false, WebCrawler::PageParserHelpers::isHttpOrHttpsScheme(QUrl("httpst://somewhere.com/index.html"))); // httpst
	EXPECT_EQ(false, WebCrawler::PageParserHelpers::isHttpOrHttpsScheme(QUrl("ftp://somewhere.com/index.html"))); // ftp
	EXPECT_EQ(false, WebCrawler::PageParserHelpers::isHttpOrHttpsScheme(QUrl("mailto:nobody@nowhere.com"))); // mailto
	EXPECT_EQ(false, WebCrawler::PageParserHelpers::isHttpOrHttpsScheme(QUrl("data:image/gif;base64"))); // data:
	EXPECT_EQ(false, WebCrawler::PageParserHelpers::isHttpOrHttpsScheme(QUrl("skype:@vasya"))); // skype:
}

}
