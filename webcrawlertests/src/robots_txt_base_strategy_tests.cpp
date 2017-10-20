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
		QString("Disallow : /stat/\n") +
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
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl("http://a.com/editemail/f/a/g/c/sss"), WebCrawler::UserAgentType::GoogleBot, tokenizer));
	EXPECT_EQ(false, baseStrategy.isUrlAllowed(QUrl("http://a.com/js"), WebCrawler::UserAgentType::GoogleBot, tokenizer));
}

TEST(RobotsTxtBaseStrategyTests, RegularExpressionsRobotsTxt)
{
	QString robotsTxt =
		QString("User-agent: *\n") +
		QString("Disallow : /api*html\n") +
		QString("Disallow : */api/*/ext$\n") +
		QString("Disallow : *js$\n");

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

	// TODO: test bad patterns
	// TODO: test robots txt that has Google Bot record and Any Bot record
	// TODO: test robots txt that has no required bot record
	// TODO: test invalid robots txt
}
}
