#include "robots_txt_tokenizer.h"
#include "robots_txt_base_strategy.h"
#include "robots_txt_rules.h"

namespace WebCrawlerTests
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

	WebCrawler::RobotsTxtTokenizer tokenizer;
	tokenizer.tokenize(robotsTxt);

	WebCrawler::RobotsTxtBaseStrategy baseStrategy;

	EXPECT_EQ(true, baseStrategy.isUrlAllowed(QUrl("http://a.com/index.html"), WebCrawler::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl("http://a.com/s/xxx"), WebCrawler::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(QUrl("http://a.com/s/cc/xxx"), WebCrawler::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(QUrl("http://a.com/sta"), WebCrawler::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(QUrl("http://a.com/stat"), WebCrawler::UserAgentType::GoogleBot, tokenizer)); // ???????
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl("http://a.com/stat/"), WebCrawler::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl("http://a.com/stat/xxx"), WebCrawler::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl("http://a.com/api"), WebCrawler::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl("http://a.com/API"), WebCrawler::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl("http://a.com/editemail/f/a/g/c/sss"), WebCrawler::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl("http://a.com/js"), WebCrawler::UserAgentType::GoogleBot, tokenizer));
}

TEST(RobotsTxtBaseStrategyTests, RegularExpressionsRobotsTxt)
{
	QString robotsTxt =
		QString("User-agent: *\n") +
		QString("Disallow : /api*html\n") +
		QString("Disallow : */api/*/ext$\n") +
		QString("Disallow : *js$\n") + 
		QString("Disallow: /example*$\n");

	WebCrawler::RobotsTxtTokenizer tokenizer;
	tokenizer.tokenize(robotsTxt);

	WebCrawler::RobotsTxtBaseStrategy baseStrategy;

	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl("http://a.com/js.js"), WebCrawler::UserAgentType::GoogleBot, tokenizer)); // Disallow : *js$
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl("http://a.com/a/b/js.js"), WebCrawler::UserAgentType::GoogleBot, tokenizer)); // Disallow : *js$
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(QUrl("http://a.com/js.jsa"), WebCrawler::UserAgentType::GoogleBot, tokenizer)); // // Disallow : *js$


	EXPECT_EQ(true, baseStrategy.isUrlAllowed(QUrl("http://a.com/a/api/a/b/js.extcss"), WebCrawler::UserAgentType::GoogleBot, tokenizer)); // Disallow : */api/*/ext$
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl("http://a.com/a/api/a/b/ext"), WebCrawler::UserAgentType::GoogleBot, tokenizer)); // Disallow : */api/*/ext$
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(QUrl("http://a.com/a/c/a/b/js.ext"), WebCrawler::UserAgentType::GoogleBot, tokenizer)); // Disallow : */api/*/ext$
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(QUrl("http://a.com/ext"), WebCrawler::UserAgentType::GoogleBot, tokenizer)); // Disallow : */api/*/ext$


	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl("http://a.com/a/api/a/b/js.html"), WebCrawler::UserAgentType::GoogleBot, tokenizer)); // Disallow : /api*html
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl("http://a.com/a/api/a/b/html/a/b/c/d.css"), WebCrawler::UserAgentType::GoogleBot, tokenizer)); // Disallow : /api*html
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl("http://a.com/a/api/a/b/js.htmlext"), WebCrawler::UserAgentType::GoogleBot, tokenizer)); // Disallow : /api*html
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(QUrl("http://a.com/a/a/b/js.htmlext"), WebCrawler::UserAgentType::GoogleBot, tokenizer)); // Disallow : /api*html
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(QUrl("http://a.com/index.html"), WebCrawler::UserAgentType::GoogleBot, tokenizer)); // Disallow : /api*html

	EXPECT_EQ(true, baseStrategy.isUrlAllowed(QUrl("http://a.com/example/index.html"), WebCrawler::UserAgentType::GoogleBot, tokenizer)); // Disallow: /example*$
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(QUrl("http://a.com/example"), WebCrawler::UserAgentType::GoogleBot, tokenizer)); // Disallow: /example*$
	

	// TODO: test robots txt that has Google Bot record and Any Bot record
	// TODO: test robots txt that has no required bot record
}

TEST(RobotsTxtBaseStrategyTests, BadPatternsRobotsTxt)
{
	QString robotsTxt =
		QString("User-agent: *\n") +
		QString("Disallow : /api*htm$l\n") +
		QString("Disallow : /корзина");

	WebCrawler::RobotsTxtTokenizer tokenizer;
	tokenizer.tokenize(robotsTxt);

	WebCrawler::RobotsTxtBaseStrategy baseStrategy;

	EXPECT_EQ(true, baseStrategy.isUrlAllowed(QUrl("http://a.com/api/index.html"), WebCrawler::UserAgentType::GoogleBot, tokenizer)); // Disallow : /api*htm$l
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(QUrl(QString::fromWCharArray(L"http://a.com/корзина")), WebCrawler::UserAgentType::GoogleBot, tokenizer)); // "Disallow : /корзина"
}


TEST(RobotsTxtBaseStrategyTests, DifferentLineSeparatorsRobotsTxt)
{
	QString robotsTxtRN =
		QString("User-agent: *\r\n") +
		QString("Disallow : /api*html\r\n");

	WebCrawler::RobotsTxtTokenizer tokenizerRN;
	tokenizerRN.tokenize(robotsTxtRN);

	WebCrawler::RobotsTxtBaseStrategy baseStrategyRN;

	EXPECT_EQ(false, baseStrategyRN.isUrlAllowed(QUrl("http://a.com/api/index.html"), WebCrawler::UserAgentType::GoogleBot, tokenizerRN));
	EXPECT_EQ(true, baseStrategyRN.isUrlAllowed(QUrl("http://a.com/index.html"), WebCrawler::UserAgentType::GoogleBot, tokenizerRN));


	QString robotsTxtNR =
		QString("User-agent: *\n\r") +
		QString("Disallow : /api*html\n\r");

	WebCrawler::RobotsTxtTokenizer tokenizerNR;
	tokenizerNR.tokenize(robotsTxtNR);

	WebCrawler::RobotsTxtBaseStrategy baseStrategyNR;

	EXPECT_EQ(false, baseStrategyNR.isUrlAllowed(QUrl("http://a.com/api/index.html"), WebCrawler::UserAgentType::GoogleBot, tokenizerNR));
	EXPECT_EQ(true, baseStrategyNR.isUrlAllowed(QUrl("http://a.com/index.html"), WebCrawler::UserAgentType::GoogleBot, tokenizerNR));


	QString robotsTxtR =
		QString("User-agent: *\r") +
		QString("Disallow : /api*html\r");

	WebCrawler::RobotsTxtTokenizer tokenizerR;
	tokenizerR.tokenize(robotsTxtR);

	WebCrawler::RobotsTxtBaseStrategy baseStrategyR;

	EXPECT_EQ(false, baseStrategyR.isUrlAllowed(QUrl("http://a.com/api/index.html"), WebCrawler::UserAgentType::GoogleBot, tokenizerR));
	EXPECT_EQ(true, baseStrategyR.isUrlAllowed(QUrl("http://a.com/index.html"), WebCrawler::UserAgentType::GoogleBot, tokenizerR));
}

TEST(RobotsTxtBaseStrategyTests, InvalidRobotsTxt)
{
	QString robotsTxt =
		// no user-agent
		QString("Disallow : /api*html\n");


	WebCrawler::RobotsTxtTokenizer tokenizer;
	tokenizer.tokenize(robotsTxt);

	WebCrawler::RobotsTxtBaseStrategy baseStrategy;

	EXPECT_EQ(true, baseStrategy.isUrlAllowed(QUrl("http://a.com/api/index.html"), WebCrawler::UserAgentType::GoogleBot, tokenizer));
}

TEST(RobotsTxtBaseStrategyTests, NonAsciiSymbolsRobotsTxt)
{
	QString robotsTxt =
		QString("User-agent: *\n") +
		QString("Disallow : /%D0%BA%D0%BE%D1%80%D0%B7%D0%B8%D0%BD%D0%B0");

	WebCrawler::RobotsTxtTokenizer tokenizer;
	tokenizer.tokenize(robotsTxt);

	WebCrawler::RobotsTxtBaseStrategy baseStrategy;

	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl("http://a.com/%D0%BA%D0%BE%D1%80%D0%B7%D0%B8%D0%BD%D0%B0"), WebCrawler::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl(QString::fromWCharArray(L"http://a.com/корзина")), WebCrawler::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl(QString::fromWCharArray(L"http://a.com/кќрзина")), WebCrawler::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(true, baseStrategy.isUrlAllowed(QUrl(QString::fromWCharArray(L"http://a.com/кјрзина")), WebCrawler::UserAgentType::GoogleBot, tokenizer));
}


}
