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
	virtual int marginTop(const QModelIndex& index) const noexcept = 0;
	virtual int marginBottom(const QModelIndex& index) const noexcept = 0;
	virtual int marginLeft(const QModelIndex& index) const noexcept = 0;
	virtual int marginRight(const QModelIndex& index) const noexcept = 0;

	virtual const QPixmap& itemPixmap(const QModelIndex& index) const noexcept = 0;
	virtual QRect itemPixmapPosition(const QModelIndex& index) const noexcept = 0;

	virtual const QColor& selectionBackgroundColor(const QModelIndex& index) const noexcept = 0;
	virtual const QColor& hoveredBackgroundColor(const QModelIndex& index) const noexcept = 0;
	virtual const QColor& backgroundColor(const QModelIndex& index) const noexcept = 0;

	virtual const QFont& font(const QModelIndex& index) const noexcept = 0;
	virtual Qt::AlignmentFlag textAlignment(const QModelIndex& index) const noexcept = 0;
	virtual QColor textColor(const QModelIndex& index) const noexcept = 0;

	virtual void invalidateRenderersCache() const noexcept = 0;
	virtual QList<const IRenderer*> renderers(const QModelIndex& index) const noexcept = 0;

	virtual void setHoveredIndex(const QModelIndex& index) noexcept = 0;
	virtual const QModelIndex& hoveredIndex() const noexcept = 0;

	virtual QObject* qobject() noexcept = 0;

protected:
	enum RendererType
	{
		PlainTextRendererType,
		UrlRendererType,
		BackgroundRendererType,
		SelectionBackgroundRendererType
	};
};

}
