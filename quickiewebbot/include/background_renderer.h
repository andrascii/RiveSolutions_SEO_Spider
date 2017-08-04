#pragma once

#include "irenderer.h"

namespace QuickieWebBot
{

class BackgroundRenderer : public IRenderer
{
public:
	virtual void render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

}