#pragma once

#include "iflexible_columns.h"

namespace SeoSpider
{

class DataExtractionColumns : public QObject, public IFlexibleColumns
{
	Q_OBJECT

public:
	DataExtractionColumns();

	virtual QString data(const CrawlerEngine::ParsedPage* page, int columnIndex) const noexcept override;
	virtual int columnCount() const noexcept override;
	virtual const QString& columnDescription(int columnIndex) const noexcept override;
	virtual int columnWidth(int columnIndex) const noexcept override;
	virtual QObject* qobject() const override;

signals:
	virtual void columnCountChanged(int newColumnCount) override;

private slots:
	void onSomeExtractorRuleTypeChanged();

private:
	QVector<QString> m_columnNames;
};

}