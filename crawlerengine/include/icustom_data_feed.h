#pragma once

#include "parsed_page.h"

namespace CrawlerEngine
{
	

class ICustomDataFeed;

class ICustomDataFeedRow
{
public:
	virtual ~ICustomDataFeedRow() = default;
	virtual QString data(const QString& column) const = 0;
	virtual const ICustomDataFeed* dataFeed() const = 0;
	virtual ParsedPageWeakPtr page() const = 0;
};

class ICustomDataFeedAuthenticator
{
public:
	virtual ~ICustomDataFeedAuthenticator() = default;
	virtual bool authenticated() const = 0;
	virtual void authenticate() = 0;
	// signals
	virtual void authenticationDone(const QVariantMap& data) = 0;
};

Q_DECLARE_METATYPE(ICustomDataFeedRow*);

class ICustomDataFeed
{
public:
	virtual ~ICustomDataFeed() = default;
	virtual DataFeedId dataFeedId() const = 0;
	virtual QString name() const = 0;
	virtual QStringList columns() const = 0;
	virtual void requestData(ParsedPageWeakPtr page) = 0;
	virtual bool connected() const = 0;
	virtual void connect() = 0;
	virtual void setConnectionData(const QVariantMap& data) = 0;
	virtual ICustomDataFeedAuthenticator* authenticator() const = 0;
	virtual QVariantMap connectionData() const = 0;
	virtual void setParams(const QVariantMap& params) = 0;

	// signals
	virtual void dataReady(ICustomDataFeedRow* row) = 0;
};

inline QString dataFeedColumnName(const ICustomDataFeed* dataFeed, int column)
{
	return dataFeed->columns()[column];
}

inline QString dataFeedColumnValue(const ICustomDataFeedRow* dataFeedRow, int column)
{
	return dataFeedRow->data(dataFeedColumnName(dataFeedRow->dataFeed(), column));
}

}