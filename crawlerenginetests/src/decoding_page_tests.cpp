#include "test_environment.h"
#include "counted_ptr.h"
#include "myhtml_parser.h"
#include "gumbo_html_parser.h"
#include "response_headers.h"

using namespace CrawlerEngine;
using namespace CrawlerEngineTests;

namespace
{

QByteArray pageData(const QString& page)
{
	QDir testDataDir;
	testDataDir.cdUp();
	testDataDir.cdUp();

	QString requestedFilePath = QDir::cleanPath(testDataDir.absolutePath() + "/crawlerenginetests/testsdata/decoding-page.com/" + page);

	QFile html(requestedFilePath);
	html.open(QIODevice::ReadOnly);

	return html.readAll();
}

QString test(const std::unique_ptr<IHtmlParser>& htmlParser, const QByteArray& html, const ResponseHeaders& responseHeaders)
{
	htmlParser->parseHtmlPage(html, responseHeaders);
	return htmlParser->currentPageEncoding().toLower();
}

}

namespace CrawlerEngineTests
{

/*
*
* Behavior:
* 1. Firstly check in response headers value of "content-type"
* 2. If "content-type" is not presented then search meta tag with attribute
	 http-equiv="content-type" and extract data from "content" attribute in that meta tag.
	 Or find <meta charset="encoding name">
* 3. If there is no meta tags, try out detect charset by html page content
*
*/

TEST(DecodingPageTests, MyHtmlParserRulesTest)
{
	ResponseHeaders responseHeaders;
	responseHeaders.addHeaderValue("content-type", "text/html; charset=utf-8");
	auto result = test(std::make_unique<MyHtmlParser>(), pageData("/windows-1251.html"), responseHeaders);
	EXPECT_EQ(true, result == QString("utf-8") || result == QString("csutf8"));

	responseHeaders.removeHeaderValues("content-type");
	responseHeaders.addHeaderValue("content-type", "text/html; charset=windows-1251");
	result = test(std::make_unique<MyHtmlParser>(), pageData("/utf8.html"), responseHeaders);
	EXPECT_EQ(true, result == QString("windows-1251") || result == QString("cswindows1251"));

	responseHeaders.removeHeaderValues("content-type");
	result = test(std::make_unique<MyHtmlParser>(), pageData("/utf8.html"), responseHeaders);
	EXPECT_EQ(true, result == QString("utf-8") || result == QString("csutf8"));

	result = test(std::make_unique<MyHtmlParser>(), pageData("/windows-1251.html"), responseHeaders);
	EXPECT_EQ(true, result == QString("windows-1251") || result == QString("cswindows1251"));
}

TEST(DecodingPageTests, GumboHtmlParserRulesTest)
{
	ResponseHeaders responseHeaders;
	responseHeaders.addHeaderValue("content-type", "text/html; charset=utf-8");
	auto result = test(std::make_unique<GumboHtmlParser>(), pageData("/windows-1251.html"), responseHeaders);
	EXPECT_EQ(QString("utf-8"), result);

	responseHeaders.removeHeaderValues("content-type");
	responseHeaders.addHeaderValue("content-type", "text/html; charset=windows-1251");
	result = test(std::make_unique<GumboHtmlParser>(), pageData("/utf8.html"), responseHeaders);
	EXPECT_EQ(QString("windows-1251"), result);

	responseHeaders.removeHeaderValues("content-type");
	result = test(std::make_unique<GumboHtmlParser>(), pageData("/utf8.html"), responseHeaders);
	EXPECT_EQ(QString("utf-8"), result);

	result = test(std::make_unique<GumboHtmlParser>(), pageData("/windows-1251.html"), responseHeaders);
	EXPECT_EQ(QString("windows-1251"), result);
}

}