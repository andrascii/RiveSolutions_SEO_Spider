#pragma once

#include "text_renderer.h"

namespace QuickieWebBot
{

class IViewModel;

class UrlRenderer : public TextRenderer
{
public:
	explicit UrlRenderer(const IViewModel* viewModel, int maxCacheSize = 0);

private:
	virtual QRect paintDecorator(QPainter* painter, const QModelIndex& index, const QRect& rect) const override;

private:
	const IViewModel* m_viewModel;
};

}