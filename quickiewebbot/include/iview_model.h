#pragma once

namespace QuickieWebBot
{

class IRenderer;
class IResizePolicy;

//
// This interface represents Qt Model extension and it's formed
// with Qt View and Qt Model MVVM pattern. Its methods used by delegates.
//

class IViewModel
{
public:
	virtual void resetRenderersCache() const noexcept = 0;
	virtual QList<IRenderer*> renderers(const QModelIndex& index) const noexcept = 0;
	virtual void setHoveredIndex(const QModelIndex& index) noexcept = 0;
	virtual const QModelIndex& hoveredIndex() const noexcept = 0;
};

}
