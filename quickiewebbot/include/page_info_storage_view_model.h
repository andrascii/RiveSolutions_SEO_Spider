#pragma once

#include "iview_model.h"

namespace QuickieWebBot
{

class DefaultColumnResizePolicy;
class PageInfoStorageModel;
class TextRenderer;
class UrlRenderer;
class SelectionBackgroundRenderer;
class BackgroundRenderer;

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

	virtual void resetRenderersCache() const noexcept override;
	virtual QList<IRenderer*> renderers(const QModelIndex& index) const noexcept override;

	virtual void setHoveredIndex(const QModelIndex& index) noexcept override;
	virtual const QModelIndex& hoveredIndex() const noexcept override;

private slots:
	void onAttachedModelInternalDataChanged();

private:
	PageInfoStorageModel* m_model;

	std::unique_ptr<TextRenderer> m_textRenderer;
	std::unique_ptr<UrlRenderer> m_urlRenderer;
	std::unique_ptr<SelectionBackgroundRenderer> m_selectionBackgroundRenderer;
	std::unique_ptr<BackgroundRenderer> m_backgroundRenderer;

	QModelIndex m_hoveredIndex;

	QColor m_selectionBgColor;
	QColor m_hoveredBgColor;
	QColor m_bgColor;

	QPixmap m_urlIcon;
};

}