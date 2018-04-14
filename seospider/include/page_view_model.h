#pragma once

#include "abstract_view_model.h"
#include "item_renderer.h"

namespace SeoSpider
{

class IView;
class PageModel;

class PageViewModel : public AbstractViewModel
{
	Q_OBJECT

	Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
	Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)
	Q_PROPERTY(QColor selectionBackgroundColor READ selectionBackgroundColor WRITE setSelectionBackgroundColor NOTIFY selectionBackgroundColorChanged)
	Q_PROPERTY(QColor hoveredBackgroundColor READ hoveredBackgroundColor WRITE setHoveredBackgroundColor NOTIFY hoveredBackgroundColorChanged)
	Q_PROPERTY(QColor selectedGridLineColor READ selectedGridLineColor WRITE setSelectedGridLineColor NOTIFY selectedGridLineColorChanged)
	Q_PROPERTY(QColor gridLineColor READ gridLineColor WRITE setGridLineColor NOTIFY gridLineColorChanged)
	Q_PROPERTY(QColor urlTextColor READ urlTextColor WRITE setUrlTextColor NOTIFY urlTextColorChanged)
	Q_PROPERTY(QFont textFont READ textFont WRITE setTextFont NOTIFY textFontChanged)

public:
	PageViewModel(QWidget* parentView, PageModel* model, QObject* parent = nullptr);

	virtual int marginTop(const QModelIndex& index) const noexcept override;
	virtual int marginBottom(const QModelIndex& index) const noexcept override;
	virtual int marginLeft(const QModelIndex& index) const noexcept override;
	virtual int marginRight(const QModelIndex& index) const noexcept override;
	virtual QPixmap pixmap(const QModelIndex& index) const noexcept override;
	virtual QRect pixmapPosition(const QModelIndex& index, const QRect& itemVisualRect) const noexcept override;
	virtual QString displayData(const QModelIndex& index, const QRect& itemVisualRect) const noexcept override;
	virtual QRect displayDataPosition(const QModelIndex& index, const QRect& itemVisualRect) const noexcept override;
	virtual const QColor& selectedBackgroundColor(const QModelIndex& index) const noexcept override;
	virtual const QColor& hoveredBackgroundColor(const QModelIndex& index) const noexcept override;
	virtual const QColor& backgroundColor(const QModelIndex& index) const noexcept override;
	virtual const QFont& font(const QModelIndex& index) const noexcept override;
	virtual int textAlignment(const QModelIndex& index) const noexcept override;
	virtual const QColor& textColor(const QModelIndex& index) const noexcept override;
	virtual const QColor& selectedGridLineColor(const QModelIndex& index) const noexcept override;
	virtual const QColor& gridLineColor(const QModelIndex& index) const noexcept override;
	virtual void setHoveredIndex(const QModelIndex& index) noexcept override;

	const QColor& backgroundColor() const noexcept;
	Q_SLOT void setBackgroundColor(const QColor& color) noexcept;
	Q_SIGNAL void backgroundColorChanged();

	const QColor& textColor() const noexcept;
	Q_SLOT void setTextColor(const QColor& color) noexcept;
	Q_SIGNAL void textColorChanged();

	const QColor& selectionBackgroundColor() const noexcept;
	Q_SLOT void setSelectionBackgroundColor(const QColor& color) noexcept;
	Q_SIGNAL void selectionBackgroundColorChanged();

	const QColor& hoveredBackgroundColor() const noexcept;
	Q_SLOT void setHoveredBackgroundColor(const QColor& color) noexcept;
	Q_SIGNAL void hoveredBackgroundColorChanged();

	const QColor& selectedGridLineColor() const noexcept;
	Q_SLOT void setSelectedGridLineColor(const QColor& color) noexcept;
	Q_SIGNAL void selectedGridLineColorChanged();

	const QColor& gridLineColor() const noexcept;
	Q_SLOT void setGridLineColor(const QColor& color) noexcept;
	Q_SIGNAL void gridLineColorChanged();

	const QColor& urlTextColor() const noexcept;
	Q_SLOT void setUrlTextColor(const QColor& color) noexcept;
	Q_SIGNAL void urlTextColorChanged();

	const QFont& textFont() const noexcept;
	Q_SLOT void setTextFont(const QFont& font) noexcept;
	Q_SIGNAL void textFontChanged();

private slots:
	void onAttachedModelStorageAdapterChanged();
	void onAttachedModelDataChanged(const QModelIndex& startIndex, const QModelIndex& endIndex, const QVector<int>& roles);
	void onModelDataWasReset();

private:
	void initializeRenderers();

private:
	QWidget* m_parentView;
	QColor m_selectionBackgroundColor;
	QColor m_hoveredBackgroundColor;
	QColor m_backgroundColor;
	QColor m_selectedGridLineColor;
	QColor m_gridLineColor;
	QColor m_urlTextColor;
	QColor m_textColor;
	QFont m_textFont;
	QPixmap m_urlIcon;
	ItemRenderer m_itemRenderer;
};

}