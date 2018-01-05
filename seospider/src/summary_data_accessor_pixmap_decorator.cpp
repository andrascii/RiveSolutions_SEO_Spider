#include "summary_data_accessor_pixmap_decorator.h"
#include "helpers.h"

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

	auto pixmapIterator = m_pixmaps.find(errorCategoryLevel(index.row()));

	DEBUG_ASSERT(pixmapIterator != m_pixmaps.end());

	return *pixmapIterator;
}

ErrorCategory::ErrorCategoryLevel SummaryDataAccessorPixmapDecorator::errorCategoryLevel(int row) const noexcept
{
	StorageType storageType = storageTypeByRow(row);

	ASSERT(storageType != StorageType::BeginEnumStorageType && 
		storageType != StorageType::EndEnumStorageType);

	return ErrorCategory::level(storageType);
}

StorageType SummaryDataAccessorPixmapDecorator::storageTypeByRow(int row) const noexcept
{
	DEBUG_ASSERT(row < SummaryDataAccessorDecorator::rowCount());

	const DCStorageDescription* storageDesc =
		SummaryDataAccessorDecorator::storageDescriptionByRow(row);

	if (!storageDesc)
	{
		return StorageType::BeginEnumStorageType;
	}

	return storageDesc->storageType;
}

void SummaryDataAccessorPixmapDecorator::initializePixmaps()
{
	QVector<QString> paths
	{
		{ QStringLiteral(":/images/icon-info.svg") },
		{ QStringLiteral(":/images/icon-ok.svg") },
		{ QStringLiteral(":/images/icon-warning.svg") },
		{ QStringLiteral(":/images/icon-error.svg") },
	};

	m_pixmaps[ErrorCategory::ErrorCategoryLevel::LevelInfo] = QPixmap(Common::Helpers::pointsToPixels(13.5), Common::Helpers::pointsToPixels(13.5));
	m_pixmaps[ErrorCategory::ErrorCategoryLevel::LevelNotError] = QPixmap(Common::Helpers::pointsToPixels(13.5), Common::Helpers::pointsToPixels(13.5));
	m_pixmaps[ErrorCategory::ErrorCategoryLevel::LevelWarning] = QPixmap(Common::Helpers::pointsToPixels(13.5), Common::Helpers::pointsToPixels(13.5));
	m_pixmaps[ErrorCategory::ErrorCategoryLevel::LevelError] = QPixmap(Common::Helpers::pointsToPixels(13.5), Common::Helpers::pointsToPixels(13.5));

	QSvgRenderer svgRenderer;

	for (int i = 0; i < paths.size(); ++i)
	{
		const ErrorCategory::ErrorCategoryLevel level = static_cast<ErrorCategory::ErrorCategoryLevel>(i);

		m_pixmaps[level].fill(Qt::transparent);
		QPainter painterPixmap(&m_pixmaps[level]);

		svgRenderer.load(paths[i]);
		svgRenderer.render(&painterPixmap);
	}
}

void SummaryDataAccessorPixmapDecorator::interceptDecoratingObjectSignal(int row, int column, Qt::ItemDataRole)
{
	Q_UNUSED(column);

	Q_EMIT dataChanged(row, 0, Qt::DecorationRole);
}

}