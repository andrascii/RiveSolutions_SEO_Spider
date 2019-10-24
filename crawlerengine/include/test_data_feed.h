#pragma once

#include "icustom_data_feed.h"
#include "parsed_page.h"

namespace CrawlerEngine
{

class TestDataFeedRow : public ICustomDataFeedRow
{
public:
	TestDataFeedRow(ICustomDataFeed* dataFeed, const QMap<QString, QString>& data, ParsedPageWeakPtr page)
		: m_dataFeed(dataFeed)
		, m_data(data)
		, m_page(page)
	{
	}

	virtual QString data(const QString& column) const override
	{
		return m_data.value(column, QString());
	}
	
	virtual const ICustomDataFeed* dataFeed() const override
	{
		return m_dataFeed;
	}
	
	virtual ParsedPageWeakPtr page() const override
	{
		return m_page;
	}

private:
	ICustomDataFeed* m_dataFeed;
	QMap<QString, QString> m_data;
	ParsedPageWeakPtr m_page;
};
	
class TestDataFeed : public QObject, public ICustomDataFeed
{
	Q_OBJECT

public:
	TestDataFeed() : QObject(nullptr) {}

	virtual DataFeedId dataFeedId() const override
	{
		return DataFeedId::TestDataFeed;
	}
	
	virtual QString name() const override
	{
		return QObject::tr("Test Data Feed");
	}

	virtual QStringList columns() const override
	{
		return QStringList() << QString("Column1") << QString("Column2");
	}

	virtual void requestData(ParsedPageWeakPtr page) override
	{
		QMap<QString, QString> data;
		data[QString("Column1")] = QString("Value1");
		data[QString("Column2")] = QString("Value2");
		
		TestDataFeedRow* row = new TestDataFeedRow(this, data, page);
		emit dataReady(row);
	}

	virtual bool connected() const override
	{
		return true;
	}
	
	virtual void connect() override
	{
	}
	
	virtual QVariantMap connectionData() const override
	{
		return QVariantMap();
	}
	
	virtual void setConnectionData(const QVariantMap& data) override
	{
		Q_UNUSED(data);
	}

	virtual ICustomDataFeedAuthenticator* authenticator() const override
	{
		return nullptr;
	}

	virtual void setParams(const QVariantMap&) override
	{
	}

	// signals
	Q_SIGNAL void dataReady(CrawlerEngine::ICustomDataFeedRow* row);
};

}