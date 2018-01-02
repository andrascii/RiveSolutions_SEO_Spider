#include "robots_txt_tokenizer.h"
#include "robots_txt_base_strategy.h"
#include "robots_txt_rules.h"

namespace CrawlerEngineTests
{
	
TEST(RobotsTxtBaseStrategyTests, SimpleRobotsTxt)
{
	QString robotsTxt =
		QString("User-agent: *\n") +
		QString("Disallow: /s/\n") +
		QString("Allow : /s/cc/\n") +
		QString("Disallow : /Stat/\n") +
		QString("Disallow : /api\n") +
		QString("Disallow : /editemail\n") +
		QString("Disallow : /js\n");

	CrawlerEngine::RobotsTxtTokenizer tokenizer;
	tokenizer.tokenize(robotsTxt);

	CrawlerEngine::RobotsTxtBaseStrategy baseStrategy;

	EXPECT_EQ(true, baseStrategy.isUrlAllowed(Url("http://a.com/index.html"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(Url("http://a.com/s/xxx"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(Url("http://a.com/s/cc/xxx"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(Url("http://a.com/sta"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(Url("http://a.com/stat"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer)); // ???????
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(Url("http://a.com/stat/"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(Url("http://a.com/stat/xxx"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(Url("http://a.com/api"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(Url("http://a.com/API"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(Url("http://a.com/editemail/f/a/g/c/sss"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(Url("http://a.com/js"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer));
}

TEST(RobotsTxtBaseStrategyTests, RegularExpressionsRobotsTxt)
{
	QString robotsTxt =
		QString("User-agent: *\n") +
		QString("Disallow : /api*html\n") +
		QString("Disallow : */api/*/ext$\n") +
		QString("Disallow : *js$\n") + 
		QString("Disallow: /example*$\n");

	CrawlerEngine::RobotsTxtTokenizer tokenizer;
	tokenizer.tokenize(robotsTxt);

	CrawlerEngine::RobotsTxtBaseStrategy baseStrategy;

	EXPECT_EQ(false, baseStrategy.isUrlAllowed(Url("http://a.com/js.js"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer)); // Disallow : *js$
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(Url("http://a.com/a/b/js.js"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer)); // Disallow : *js$
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(Url("http://a.com/js.jsa"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer)); // // Disallow : *js$


	EXPECT_EQ(true, baseStrategy.isUrlAllowed(Url("http://a.com/a/api/a/b/js.extcss"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer)); // Disallow : */api/*/ext$
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(Url("http://a.com/a/api/a/b/ext"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer)); // Disallow : */api/*/ext$
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(Url("http://a.com/a/c/a/b/js.ext"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer)); // Disallow : */api/*/ext$
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(Url("http://a.com/ext"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer)); // Disallow : */api/*/ext$


	EXPECT_EQ(false, baseStrategy.isUrlAllowed(Url("http://a.com/a/api/a/b/js.html"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer)); // Disallow : /api*html
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(Url("http://a.com/a/api/a/b/html/a/b/c/d.css"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer)); // Disallow : /api*html
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(Url("http://a.com/a/api/a/b/js.htmlext"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer)); // Disallow : /api*html
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(Url("http://a.com/a/a/b/js.htmlext"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer)); // Disallow : /api*html
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(Url("http://a.com/index.html"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer)); // Disallow : /api*html

	EXPECT_EQ(true, baseStrategy.isUrlAllowed(Url("http://a.com/example/index.html"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer)); // Disallow: /example*$
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(Url("http://a.com/example"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer)); // Disallow: /example*$
	

	// TODO: test robots txt that has Google Bot record and Any Bot record
	// TODO: test robots txt that has no required bot record
}

TEST(RobotsTxtBaseStrategyTests, BadPatternsRobotsTxt)
{
	QString robotsTxt =
		QString("User-agent: *\n") +
		QString("Disallow : /api*htm$l\n") +
		QString("Disallow : /корзина");

	CrawlerEngine::RobotsTxtTokenizer tokenizer;
	tokenizer.tokenize(robotsTxt);

	CrawlerEngine::RobotsTxtBaseStrategy baseStrategy;

	EXPECT_EQ(true, baseStrategy.isUrlAllowed(Url("http://a.com/api/index.html"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer)); // Disallow : /api*htm$l
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(Url(QString::fromWCharArray(L"http://a.com/корзина")), CrawlerEngine::UserAgentType::GoogleBot, tokenizer)); // "Disallow : /корзина"
}


TEST(RobotsTxtBaseStrategyTests, DifferentLineSeparatorsRobotsTxt)
{
	QString robotsTxtRN =
		QString("User-agent: *\r\n") +
		QString("Disallow : /api*html\r\n");

	CrawlerEngine::RobotsTxtTokenizer tokenizerRN;
	tokenizerRN.tokenize(robotsTxtRN);

	CrawlerEngine::RobotsTxtBaseStrategy baseStrategyRN;

	EXPECT_EQ(false, baseStrategyRN.isUrlAllowed(Url("http://a.com/api/index.html"), CrawlerEngine::UserAgentType::GoogleBot, tokenizerRN));
	EXPECT_EQ(true, baseStrategyRN.isUrlAllowed(Url("http://a.com/index.html"), CrawlerEngine::UserAgentType::GoogleBot, tokenizerRN));


	QString robotsTxtNR =
		QString("User-agent: *\n\r") +
		QString("Disallow : /api*html\n\r");

	CrawlerEngine::RobotsTxtTokenizer tokenizerNR;
	tokenizerNR.tokenize(robotsTxtNR);

	CrawlerEngine::RobotsTxtBaseStrategy baseStrategyNR;

	EXPECT_EQ(false, baseStrategyNR.isUrlAllowed(Url("http://a.com/api/index.html"), CrawlerEngine::UserAgentType::GoogleBot, tokenizerNR));
	EXPECT_EQ(true, baseStrategyNR.isUrlAllowed(Url("http://a.com/index.html"), CrawlerEngine::UserAgentType::GoogleBot, tokenizerNR));


	QString robotsTxtR =
		QString("User-agent: *\r") +
		QString("Disallow : /api*html\r");

	CrawlerEngine::RobotsTxtTokenizer tokenizerR;
	tokenizerR.tokenize(robotsTxtR);

	CrawlerEngine::RobotsTxtBaseStrategy baseStrategyR;

	EXPECT_EQ(false, baseStrategyR.isUrlAllowed(Url("http://a.com/api/index.html"), CrawlerEngine::UserAgentType::GoogleBot, tokenizerR));
	EXPECT_EQ(true, baseStrategyR.isUrlAllowed(Url("http://a.com/index.html"), CrawlerEngine::UserAgentType::GoogleBot, tokenizerR));
}

TEST(RobotsTxtBaseStrategyTests, InvalidRobotsTxt)
{
	QString robotsTxt =
		// no user-agent
		QString("Disallow : /api*html\n");


	CrawlerEngine::RobotsTxtTokenizer tokenizer;
	tokenizer.tokenize(robotsTxt);

	CrawlerEngine::RobotsTxtBaseStrategy baseStrategy;

	EXPECT_EQ(true, baseStrategy.isUrlAllowed(Url("http://a.com/api/index.html"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer));
}

TEST(RobotsTxtBaseStrategyTests, NonAsciiSymbolsRobotsTxt)
{
	QString robotsTxt =
		QString("User-agent: *\n") +
		QString("Disallow : /%D0%BA%D0%BE%D1%80%D0%B7%D0%B8%D0%BD%D0%B0");

	CrawlerEngine::RobotsTxtTokenizer tokenizer;
	tokenizer.tokenize(robotsTxt);

	CrawlerEngine::RobotsTxtBaseStrategy baseStrategy;

	EXPECT_EQ(false, baseStrategy.isUrlAllowed(Url("http://a.com/%D0%BA%D0%BE%D1%80%D0%B7%D0%B8%D0%BD%D0%B0"), CrawlerEngine::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(Url(QString::fromWCharArray(L"http://a.com/корзина")), CrawlerEngine::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(Url(QString::fromWCharArray(L"http://a.com/кќрзина")), CrawlerEngine::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(Url(QString::fromWCharArray(L"http://a.com/кјрзина")), CrawlerEngine::UserAgentType::GoogleBot, tokenizer));
}


}
