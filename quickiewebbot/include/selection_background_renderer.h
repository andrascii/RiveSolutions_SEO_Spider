#pragma once

#include "background_renderer.h"

namespace QuickieWebBot
{
	
class SelectionBackgroundRenderer : public IRenderer
{
public:
	virtual void render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual void resetCache() override;
	virtual void setCacheSize(int cacheSize) override;
};

}
