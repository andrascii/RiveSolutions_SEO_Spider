#pragma once

#include "abstract_view_model.h"
#include "item_renderer.h"

namespace SeoSpider
{

class SummaryModel;
class ViewportPercentResizePolicy;

class SummaryViewModel : public AbstractViewModel
{
	Q_OBJECT

	Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
	Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)
	Q_PROPERTY(QColor selectedBackgroundColor READ selectedBackgroundColor WRITE setSelectedBackgroundColor NOTIFY selectedBackgroundColorChanged)
	Q_PROPERTY(QColor hoveredBackgroundColor READ hoveredBackgroundColor WRITE setHoveredBackgroundColor NOTIFY hoveredBackgroundColorChanged)
	Q_PROPERTY(QColor selectedGridLineColor READ selectedGridLineColor WRITE setSelectedGridLineColor NOTIFY selectedGridLineColorChanged)
	Q_PROPERTY(QColor gridLineColor READ gridLineColor WRITE setGridLineColor NOTIFY gridLineColorChanged)
	Q_PROPERTY(QColor headerTextColor READ headerTextColor WRITE setHeaderTextColor NOTIFY headerTextColorChanged)
	Q_PROPERTY(QFont headerFont READ headerFont WRITE setHeaderFont NOTIFY headerFontChanged)
	Q_PROPERTY(QFont textFont READ textFont WRITE setTextFont NOTIFY textFontChanged)

public:
	SummaryViewModel(SummaryModel* model, QObject* parent = nullptr);

	virtual int marginTop(const QModelIndex& index) const noexcept override;
	virtual int marginBottom(const QModelIndex& index) const noexcept override;
	virtual int marginLeft(const QModelIndex& index) const noexcept override;
	virtual int marginRight(const QModelIndex& index) const noexcept override;

	virtual const QPixmap& pixmap(const QModelIndex& index) const noexcept override;
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

	const QColor& selectedBackgroundColor() const noexcept;
	Q_SLOT void setSelectedBackgroundColor(const QColor& color) noexcept;
	Q_SIGNAL void selectedBackgroundColorChanged();

	const QColor& hoveredBackgroundColor() const noexcept;
	Q_SLOT void setHoveredBackgroundColor(const QColor& color) noexcept;
	Q_SIGNAL void hoveredBackgroundColorChanged();

	const QColor& selectedGridLineColor() const noexcept;
	Q_SLOT void setSelectedGridLineColor(const QColor& color) noexcept;
	Q_SIGNAL void selectedGridLineColorChanged();

	const QColor& gridLineColor() const noexcept;
	Q_SLOT void setGridLineColor(const QColor& color) noexcept;
	Q_SIGNAL void gridLineColorChanged();

	const QColor& headerTextColor() const noexcept;
	Q_SLOT void setHeaderTextColor(const QColor& color) noexcept;
	Q_SIGNAL void headerTextColorChanged();

	const QFont& headerFont() const noexcept;
	Q_SLOT void setHeaderFont(const QFont& font) noexcept;
	Q_SIGNAL void headerFontChanged();

	const QFont& textFont() const noexcept;
	Q_SLOT void setTextFont(const QFont& font) noexcept;
	Q_SIGNAL void textFontChanged();

private slots:
	void onDataWereReset();

private:
	void initializeRenderers();

private:
	QColor m_selectedBackgroundColor;
	QColor m_hoveredBackgroundColor;
	QColor m_backgroundColor;
	QColor m_selectedGridLineColor;
	QColor m_gridLineColor;

	QColor m_headerTextColor;
	QColor m_textColor;

	QFont m_headerFont;
	QFont m_textFont;

	ItemRenderer m_itemRenderer;
};

}
