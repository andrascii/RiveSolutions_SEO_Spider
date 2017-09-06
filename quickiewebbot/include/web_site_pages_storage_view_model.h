#pragma once

#include "abstract_view_model.h"

namespace QuickieWebBot
{

class WebSitePagesStorageModel;

class WebSitePagesStorageViewModel : public AbstractViewModel
{
	Q_OBJECT

public:
	WebSitePagesStorageViewModel(WebSitePagesStorageModel* model, QObject* parent = nullptr);

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

	virtual void setHoveredIndex(const QModelIndex& index) noexcept override;

	virtual QList<const IRenderer*> renderers(const QModelIndex& index) const noexcept override;

private slots:
	void onAttachedModelInternalDataChanged();

private:
	void initializeRenderers();

private:
	QColor m_selectionBgColor;
	QColor m_hoveredBgColor;
	QColor m_bgColor;

	QPixmap m_urlIcon;
};

}