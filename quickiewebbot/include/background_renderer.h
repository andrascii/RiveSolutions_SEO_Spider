#pragma once

#include "irenderer.h"

namespace QuickieWebBot
{

class BackgroundRenderer : public IRenderer
{
public:
	BackgroundRenderer(QColor color, QColor alternateColor);
	virtual void render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
	QColor m_color;
	QColor m_alternateColor;
};

}