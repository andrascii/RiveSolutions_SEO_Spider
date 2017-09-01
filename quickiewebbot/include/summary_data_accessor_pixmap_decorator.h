#pragma once

#include "summary_data_accessor_decorator.h"

namespace QuickieWebBot
{

class SummaryDataAccessorPixmapDecorator : public SummaryDataAccessorDecorator
{
	Q_OBJECT

public:
	SummaryDataAccessorPixmapDecorator(ISummaryDataAccessor* dataAccessor);

	virtual const QPixmap& pixmap(const QModelIndex& index) const noexcept override;

private:
	enum ItemStatus
	{
		StatusOK,
		StatusWarning,
		StatusError
	};

	ItemStatus itemStatus(int row) const noexcept;

	int rowByStorageType(WebCrawler::DataCollection::StorageType storageType) const noexcept;
	WebCrawler::DataCollection::GuiStorageTypePtr storageByRow(int row) const noexcept;

	void initializePixmaps();

	Q_SLOT void emitDataChanged(int row, int storageType);

private:
	QMap<ItemStatus, QPixmap> m_pixmaps;
};

}
