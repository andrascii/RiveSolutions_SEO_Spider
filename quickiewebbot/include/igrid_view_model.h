#pragma once

namespace QuickieWebBot
{

class IRenderer;
class IGridViewResizePolicy;

//
// This interface represents Qt Model extension and it's formed
// with Qt View and Qt Model MVVM pattern. Its methods used by delegates.
//

class IGridViewModel
{
public:
	virtual QList<IRenderer*> renderers(const QModelIndex& index) const noexcept = 0;
	virtual IGridViewResizePolicy* resizePolicy() const noexcept = 0;
};

}
