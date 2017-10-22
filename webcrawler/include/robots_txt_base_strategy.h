#pragma once

namespace WebCrawler
{
	enum class UserAgentType;
	class RobotsTxtTokenizer;

class RobotsTxtBaseStrategy
{
public:
	virtual bool isUrlAllowed(const QUrl& url, UserAgentType userAgentType, const RobotsTxtTokenizer& tokenizer) const;
	virtual QUrl cleanUrl(const QUrl& url, UserAgentType userAgentType, const RobotsTxtTokenizer& tokenizer) const;

protected:
	bool patternMatched(const QString& pattern, const QString value) const;

	bool checkIfSupportedRecordExistsAndCorrectUserAgentType(UserAgentType& userAgentType, const RobotsTxtTokenizer& tokenizer) const;
};

}