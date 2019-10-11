#include "counted_ptr.h"
#include "myhtml_parser.h"
#include "response_headers.h"
#include "html_node_model.h"

namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

const QByteArray html = 
"<!DOCTYPE html>"
"<html>"
"<head>"
"<title>Dummy Title</title>"
"<meta name = \"description\" content=\"Dummy Description\">"
"</head>"
"<body>"
"<h1>Dummy H1</h1>"
"<a href=\"/page-1.htm\" id=\"42\">1</a>"
"<div class=\"dv\"><div><span class=\"sp\" attr=\"sp\">99</span></div></div>"
"</body>"
"</html>";

QString evaluateXPath(const QByteArray& doc, const QString& xpath)
{
	MyHtmlParser parser;

	QXmlQuery xmlQuery(QXmlQuery::XQuery10);

	parser.parseHtmlPage(doc, ResponseHeaders());
	HtmlNodeModel model(xmlQuery.namePool(), &parser);

	QXmlNodeModelIndex startNode = model.rootIndex();
	xmlQuery.bindVariable("doc", startNode);
	xmlQuery.setQuery(xpath, QUrl("https://google.com"));

	QString result;
	xmlQuery.evaluateTo(&result);
	return result.trimmed();
}

TEST(XPathTests, Simple)
{
	EXPECT_EQ(QString("1"), evaluateXPath(html, "$doc//a/string()"));
	EXPECT_EQ(QString(""), evaluateXPath(html, "$doc//ul/string()"));
}

TEST(XPathTests, SimpleAttribute)
{
	EXPECT_EQ(QString("42"), evaluateXPath(html, "$doc//a/@id/string()"));
	EXPECT_EQ(QString(""), evaluateXPath(html, "$doc//a/@id1/string()"));
}

TEST(XPathTests, DeepElement)
{
	// BEWARE: $doc is actually html element, so html should NOT be included in XPath query!
	EXPECT_EQ(QString("99"), evaluateXPath(html, "$doc/body/div[@class=\"dv\"]/div/span[@attr=\"sp\"]/string()"));
	EXPECT_EQ(QString(""), evaluateXPath(html, "$doc/body/div[@class=\"dv2\"]/div/span[@attr=\"sp\"]/string()"));
	EXPECT_EQ(QString(""), evaluateXPath(html, "$doc/body/div[@class=\"dv\"]/span/span[@attr=\"sp\"]/string()"));
}

}
