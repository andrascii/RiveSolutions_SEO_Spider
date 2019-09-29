#include "svg_renderer.h"
#include "helpers.h"

namespace SeoSpider
{

QPixmap SvgRenderer::render(const QString& filepath, const QSize& sizeInPoints)
{
	const QSize sizeInPixels(
		Common::Helpers::pointsToPixels(sizeInPoints.width()),
		Common::Helpers::pointsToPixels(sizeInPoints.height())
	);

	const QString key = QStringLiteral("%1%2%3").arg(filepath).arg(sizeInPixels.width()).arg(sizeInPixels.height());

	QPixmap pixmap(sizeInPixels);

	if (QPixmapCache::find(key, pixmap))
	{
		return pixmap;
	}

	pixmap.fill(Qt::transparent);

	QPainter painter(&pixmap);
	QSvgRenderer renderer;

	const bool fileMustBeLoaded = renderer.load(filepath);
	DEBUG_ASSERT_WITH_DUMP(fileMustBeLoaded && "Cannot load svg image", DUMP(filepath));

	renderer.render(&painter);

	QPixmapCache::insert(key, pixmap);

	return pixmap;
}

QPixmap SvgRenderer::render(const QString& filepath, int widthInPoints, int heightInPoints)
{
	return SvgRenderer::render(filepath, QSize(widthInPoints, heightInPoints));
}

}