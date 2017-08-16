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
	virtual int marginTop() const noexcept = 0;
	virtual int marginBottom() const noexcept = 0;
	virtual int marginRight() const noexcept = 0;
	virtual int marginLeft() const noexcept = 0;

	virtual void resetRenderersCache() const noexcept = 0;
	virtual QList<IRenderer*> renderers(const QModelIndex& index) const noexcept = 0;
};

}
