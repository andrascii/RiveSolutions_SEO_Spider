#pragma once

#include "iview_model.h"

namespace QuickieWebBot
{

class PageInfoStorageModel;

class PageInfoStorageViewModel
	: public QObject
	, public IViewModel
{
	Q_OBJECT

public:
	PageInfoStorageViewModel(PageInfoStorageModel* model, QObject* parent = nullptr);

	virtual int marginTop(const QModelIndex& index) const noexcept override;
	virtual int marginBottom(const QModelIndex& index) const noexcept override;
	virtual int marginLeft(const QModelIndex& index) const noexcept override;
	virtual int marginRight(const QModelIndex& index) const noexcept override;

	virtual const QPixmap& itemPixmap(const QModelIndex& index) const noexcept override;
	virtual QRect itemPixmapPosition(const QModelIndex& index) const noexcept override;

	virtual const QColor& selectionBackgroundColor(const QModelIndex& index) const noexcept override;
	virtual const QColor& hoveredBackgroundColor(const QModelIndex& index) const noexcept override;
	virtual const QColor& backgroundColor(const QModelIndex& index) const noexcept override;

	virtual const QFont& font(const QModelIndex& index) const noexcept override;
	virtual Qt::AlignmentFlag textAlignment(const QModelIndex& index) const noexcept override;
	virtual QColor textColor(const QModelIndex& index) const noexcept override;

	virtual void invalidateRenderersCache() const noexcept override;
	virtual QList<const IRenderer*> renderers(const QModelIndex& index) const noexcept override;

	virtual void setHoveredIndex(const QModelIndex& index) noexcept override;
	virtual const QModelIndex& hoveredIndex() const noexcept override;

	virtual void setSelectedIndexes(const QModelIndexList& modelIndexes) noexcept override;
	virtual void setDeselectedIndexes(const QModelIndexList& modelIndexes) noexcept override;
	virtual const QModelIndexList& selectedIndexes() const noexcept override;

	virtual QObject* qobject() noexcept override;

	Q_SIGNAL virtual void repaintItems(const QModelIndexList& modelIndexes) const override;

private slots:
	void onAttachedModelInternalDataChanged();

private:
	void invalidateCacheIndexes(const QModelIndexList& indexesList);
	void invalidateCacheIndex(const QModelIndex& index);
	void initializeRenderers();

private:
	enum RendererType
	{
		PlainTextRendererType,
		UrlRendererType,
		BackgroundRendererType,
		SelectionBackgroundRendererType,
		GridLineRendererType
	};

private:
	PageInfoStorageModel* m_model;

	std::map<RendererType, std::unique_ptr<IRenderer>> m_renderers;

	QModelIndex m_hoveredIndex;

	QColor m_selectionBgColor;
	QColor m_hoveredBgColor;
	QColor m_bgColor;

	QPixmap m_urlIcon;

	QModelIndexList m_selectedModelIndexes;
};

}