#pragma once

#include "summary_data_accessor_decorator.h"
#include "sequenced_data_collection.h"
#include "error_category.h"

namespace SeoSpider
{

using namespace CrawlerEngine;

class SummaryDataAccessorPixmapDecorator : public SummaryDataAccessorDecorator
{
	Q_OBJECT

public:
	SummaryDataAccessorPixmapDecorator(ISummaryDataAccessor* dataAccessor);

	virtual const QPixmap& pixmap(const QModelIndex& index) const noexcept override;

private:
	ErrorCategory::ErrorCategoryLevel errorCategoryLevel(int row) const noexcept;
	CrawlerEngine::StorageType storageTypeByRow(int row) const noexcept;
	void initializePixmaps();

private slots:
	void interceptDecoratingObjectSignal(int row, int storageType, Qt::ItemDataRole);

private:
	QMap<ErrorCategory::ErrorCategoryLevel, QPixmap> m_pixmaps;
};

}
