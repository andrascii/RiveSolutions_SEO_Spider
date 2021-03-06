#include "stdafx.h"
#include "robots_txt_tokenizer.h"
#include "robots_txt_yandex_strategy.h"
#include "robots_txt_rules.h"

namespace CrawlerEngineTests
{
	
TEST(RobotsTxtYandexStrategyTests, CleanParam)
{
	QString robotsTxt =
		QString("User-agent: *\n") +
		QString("Clean-param: p1&p2&p3\n") +
		QString("Clean-param: p10&p20&p30 /path1\n");

	CrawlerEngine::RobotsTxtTokenizer tokenizer;
	tokenizer.tokenize(robotsTxt);

	CrawlerEngine::RobotsTxtYandexStrategy yandexStrategy;

	QString result = yandexStrategy.cleanUrl(Url("http://a.com/index.html?p4=6&p1=2"), 
		CrawlerEngine::UserAgentType::AnyBot, tokenizer).toDisplayString();
	EXPECT_EQ(QString("http://a.com/index.html?p4=6"), result);

	result = yandexStrategy.cleanUrl(Url("http://a.com/index.html?P4=6&P1=2"),
		CrawlerEngine::UserAgentType::AnyBot, tokenizer).toDisplayString();
	EXPECT_EQ(QString("http://a.com/index.html?P4=6"), result);

	result = yandexStrategy.cleanUrl(Url("http://a.com/index.html?P1=2"),
		CrawlerEngine::UserAgentType::AnyBot, tokenizer).toDisplayString();
	EXPECT_EQ(QString("http://a.com/index.html"), result);

	result = yandexStrategy.cleanUrl(Url("http://a.com/index.html?P10=2"),
		CrawlerEngine::UserAgentType::AnyBot, tokenizer).toDisplayString();
	EXPECT_EQ(QString("http://a.com/index.html?P10=2"), result);



	result = yandexStrategy.cleanUrl(Url("http://a.com/path1/index.html?P10=2"),
		CrawlerEngine::UserAgentType::AnyBot, tokenizer).toDisplayString();
	EXPECT_EQ(QString("http://a.com/path1/index.html"), result);

	result = yandexStrategy.cleanUrl(Url("http://a.com/path2/path1/index.html?P10=2"),
		CrawlerEngine::UserAgentType::AnyBot, tokenizer).toDisplayString();
	EXPECT_EQ(QString("http://a.com/path2/path1/index.html?P10=2"), result);
	
}

}
