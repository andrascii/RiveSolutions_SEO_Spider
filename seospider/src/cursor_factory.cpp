#include "stdafx.h"
#include "cursor_factory.h"
#include "svg_renderer.h"

namespace SeoSpider
{

QCursor CursorFactory::createCursor(Qt::CursorShape shape)
{
	switch (shape)
	{
		case Qt::ArrowCursor:
		{
			return QCursor(SvgRenderer::render(QStringLiteral(":/images/arrow-cursor.svg"), 14, 14));
		}
		case Qt::WaitCursor:
		{
			return QCursor(SvgRenderer::render(QStringLiteral(":/images/wait-cursor.svg"), 14, 14));
		}
		case Qt::SizeVerCursor:
		{
			return QCursor(SvgRenderer::render(QStringLiteral(":/images/size-ver-cursor.svg"), 14, 14));
		}
		case Qt::SizeHorCursor:
		{
			return QCursor(SvgRenderer::render(QStringLiteral(":/images/size-hor-cursor.svg"), 14, 14));
		}
		case Qt::SizeBDiagCursor:
		{
			return QCursor(SvgRenderer::render(QStringLiteral(":/images/size-bdiag-cursor.svg"), 14, 14));
		}
		case Qt::SizeFDiagCursor:
		{
			return QCursor(SvgRenderer::render(QStringLiteral(":/images/size-fdiag-cursor.svg"), 14, 14));
		}
		case Qt::SplitVCursor:
		{
			return QCursor(SvgRenderer::render(QStringLiteral(":/images/split-v-cursor.svg"), 14, 14));
		}
		case Qt::SplitHCursor:
		{
			return QCursor(SvgRenderer::render(QStringLiteral(":/images/split-h-cursor.svg"), 14, 14));
		}
		case Qt::PointingHandCursor:
		{
			return QCursor(SvgRenderer::render(QStringLiteral(":/images/pointing-hand-cursor.svg"), 14, 14), -1, 0);
		}
		case Qt::OpenHandCursor:
		{
			return QCursor(SvgRenderer::render(QStringLiteral(":/images/open-hand-cursor.svg"), 14, 14), -1, 0);
		}
		case Qt::ClosedHandCursor:
		{
			return QCursor(SvgRenderer::render(QStringLiteral(":/images/closed-hand-cursor.svg"), 14, 14));
		}
		case Qt::WhatsThisCursor:
		{
			return QCursor(SvgRenderer::render(QStringLiteral(":/images/whats-this-cursor.svg"), 14, 14));
		}
		case Qt::BusyCursor:
		{
			return QCursor(SvgRenderer::render(QStringLiteral(":/images/busy-cursor.svg"), 14, 14));
		}
	}

	return QCursor(shape);
}

}