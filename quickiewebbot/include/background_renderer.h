#pragma once

#include "irenderer.h"

namespace QuickieWebBot
{

class IViewModel;

class BackgroundRenderer : public IRenderer
{
public:
	BackgroundRenderer(const IViewModel* viewModel);

	virtual void render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual void resetCache() override;
	virtual void setCacheSize(int cacheSize) override;

private:
	const IViewModel* m_viewModel;
};

}