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
* https://www.w3.org/blog/2008/03/html-charset/
*
*
* The Recipe
* Given all these potential sources for the declaration (or automatic detection) of the document character encoding, 
* all potentially contradicting the others, what should be the recipe to reliably figure out which encoding to use?
*
* 1. The charset info in the HTTP Content-Type header should have precedence. Always
*
* 2. Next in line is the charset information in the XML declaration. Which may be there, or may not.
* For XHTML documents, and in particular for the XHTML documents served as text/html, it is recommended to avoid using an XML declaration.
* But let’s remember: XHTML is XML, and XML requires an XML declaration or some other method of declaration for XML documents
* using encodings other than UTF-8 or UTF-16 (or ascii, which is a convenient subset…).
* As a result, there is a strong likeliness that anything served as application/xhtml+xml (or text/html and looking a lot like XHTML), 
* with neither encoding declaration at the HTTP level nor in an XML declaration is quite likely to be UTF-8 or UTF-16
* Then there is the BOM, a signature for Unicode character encodings.
*
* 3. Then comes the search for the meta information that might, just might, provide a character encoding declaration.
*
* 4. Beyond that point, it’s the land of defaults and heuristics. You may choose to default to iso-8859-1 for text/html resources, utf-8 for application/xhtml+xml.
* The rest is heuristics. You could venture towards fallback encodings such as windows-1252, which many consider a safe bet, but a bet nonetheless.
*
* 5. There are quite a few algorithms to determine the likeliness of one specific encoding based on matching at byte level.
* Martin Dürst wrote a regexp to check whether a document will “fit” as utf-8. If you know other reliable algorithms, 
* feel free to mention them in the comments, I will list them here.
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