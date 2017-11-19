#include "summary_data_accessor_pixmap_decorator.h"
#include "seo_spider_helpers.h"

namespace SeoSpider
{

SummaryDataAccessorPixmapDecorator::SummaryDataAccessorPixmapDecorator(ISummaryDataAccessor* dataAccessor)
	: SummaryDataAccessorDecorator(dataAccessor)
{
	initializePixmaps();

	VERIFY(connect(dataAccessor->qobject(), SIGNAL(dataChanged(int, int, Qt::ItemDataRole)), this, 
		SLOT(interceptDecoratingObjectSignal(int, int, Qt::ItemDataRole))));
}

const QPixmap& SummaryDataAccessorPixmapDecorator::pixmap(const QModelIndex& index) const noexcept
{
	static QPixmap emptyPixmap;

	if (SummaryDataAccessorDecorator::isHeaderRow(index.row()) || index.column() != 0)
	{
		return emptyPixmap;
	}

	auto pixmapIterator = m_pixmaps.find(itemStatus(index.row()));

	DEBUG_ASSERT(pixmapIterator != m_pixmaps.end());

	return *pixmapIterator;
}

SummaryDataAccessorPixmapDecorator::ItemStatus SummaryDataAccessorPixmapDecorator::itemStatus(int row) const noexcept
{
	const CrawlerEngine::ISequencedStorage* storage = storageByRow(row);

	ASSERT(storage);

	if (storage->size() > 10)
	{
		return StatusError;
	}

	if (storage->size())
	{
		return StatusWarning;
	}

	return StatusOK;
}

const CrawlerEngine::ISequencedStorage* SummaryDataAccessorPixmapDecorator::storageByRow(int row) const noexcept
{
	DEBUG_ASSERT(row < SummaryDataAccessorDecorator::rowCount());

	const DCStorageDescription* storageDesc = 
		SummaryDataAccessorDecorator::storageDescriptionByRow(row);

	if (!storageDesc)
	{
		return nullptr;
	}

	return SummaryDataAccessorDecorator::sequencedDataCollection()->storage(storageDesc->storageType);
}

void SummaryDataAccessorPixmapDecorator::initializePixmaps()
{
	QVector<QString> paths
	{
		{ QStringLiteral(":/images/icon-ok.svg") },
		{ QStringLiteral(":/images/icon-warning.svg") },
		{ QStringLiteral(":/images/icon-error.svg") },
	};

	m_pixmaps[ItemStatus::StatusOK] = QPixmap(SeoSpiderHelpers::pointsToPixels(13.5), SeoSpiderHelpers::pointsToPixels(13.5));
	m_pixmaps[ItemStatus::StatusWarning] = QPixmap(SeoSpiderHelpers::pointsToPixels(13.5), SeoSpiderHelpers::pointsToPixels(13.5));
	m_pixmaps[ItemStatus::StatusError] = QPixmap(SeoSpiderHelpers::pointsToPixels(13.5), SeoSpiderHelpers::pointsToPixels(13.5));

	QSvgRenderer svgRenderer;

	for (int i = 0; i < paths.size(); ++i)
	{
		m_pixmaps[static_cast<ItemStatus>(i)].fill(Qt::transparent);
		QPainter painterPixmap(&m_pixmaps[static_cast<ItemStatus>(i)]);

		svgRenderer.load(paths[i]);
		svgRenderer.render(&painterPixmap);
	}
}

void SummaryDataAccessorPixmapDecorator::interceptDecoratingObjectSignal(int row, int column, Qt::ItemDataRole)
{
	Q_EMIT dataChanged(row, 0, Qt::DecorationRole);
}

}