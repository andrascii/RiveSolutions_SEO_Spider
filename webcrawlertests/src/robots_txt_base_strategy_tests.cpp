#include "robots_txt_tokenizer.h"
#include "robots_txt_base_strategy.h"
#include "robots_txt_rules.h"

namespace WebCrawlerTests
{
	
TEST(RobotsTxtBaseStrategyTests, ParseOnlyHtml)
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

}
