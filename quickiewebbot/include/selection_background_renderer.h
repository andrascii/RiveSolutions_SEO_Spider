#pragma once

#include "background_renderer.h"

namespace QuickieWebBot
{
	
class SelectionBackgroundRenderer : public BackgroundRenderer
{
public:
	SelectionBackgroundRenderer(QColor color, QColor alternateColor);
	virtual void render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

}
