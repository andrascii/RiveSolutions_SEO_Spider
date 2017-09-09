#pragma once

#include "summary_data_accessor_decorator.h"
#include "gui_storage.h"

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

	WebCrawler::GuiStorage::GuiStorageTypePtr storageByRow(int row) const noexcept;

	void initializePixmaps();

	Q_SLOT void interceptDecoratingObjectSignal(int row, int storageType, Qt::ItemDataRole);

private:
	QMap<ItemStatus, QPixmap> m_pixmaps;
};

}
