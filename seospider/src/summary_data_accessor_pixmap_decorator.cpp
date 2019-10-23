#include "stdafx.h"
#include "summary_data_accessor_pixmap_decorator.h"
#include "helpers.h"
#include "svg_renderer.h"

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
	const QSize pixmapSize(13.5, 13.5);

	m_pixmaps[ErrorCategory::ErrorCategoryLevel::LevelInfo] = SvgRenderer::render(QStringLiteral(":/images/icon-info.svg"), pixmapSize);
	m_pixmaps[ErrorCategory::ErrorCategoryLevel::LevelNotError] = SvgRenderer::render(QStringLiteral(":/images/icon-ok.svg"), pixmapSize);
	m_pixmaps[ErrorCategory::ErrorCategoryLevel::LevelWarning] = SvgRenderer::render(QStringLiteral(":/images/icon-warning.svg"), pixmapSize);
	m_pixmaps[ErrorCategory::ErrorCategoryLevel::LevelError] = SvgRenderer::render(QStringLiteral(":/images/icon-error.svg"), pixmapSize);
}

void SummaryDataAccessorPixmapDecorator::interceptDecoratingObjectSignal(int row, int column, Qt::ItemDataRole)
{
	Q_UNUSED(column);

	Q_EMIT dataChanged(row, 0, Qt::DecorationRole);
}

}