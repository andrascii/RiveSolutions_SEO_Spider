#pragma once

#include "igrid_data_accessor.h"

namespace QuickieWebBot
{

class GridModel : public QAbstractTableModel
{
	Q_OBJECT;

	Q_PROPERTY(QColor headerBackgroundColor READ headerBackgroundColor
		WRITE setHeaderBackgroundColor NOTIFY headerBackgroundColorChanged);

	Q_PROPERTY(QColor headerTextColor READ headerTextColor
		WRITE setHeaderTextColor NOTIFY headerTextColorChanged);

	Q_PROPERTY(QColor backgroundColor READ backgroundColor
		WRITE setBackgroundColor NOTIFY backgroundColorChanged);

	Q_PROPERTY(QColor textColor READ textColor
		WRITE setTextColor NOTIFY textColorChanged);

	Q_PROPERTY(QFont textFont READ textFont
		WRITE setTextFont NOTIFY textFontChanged);

	Q_PROPERTY(QColor gridSelectionBackgroundColor READ gridSelectionBackgroundColor
		WRITE setGridSelectionBackgroundColor NOTIFY gridSelectionBackgroundColorChanged);

	Q_PROPERTY(QColor gridSelectionBackgroundInactiveColor READ gridSelectionBackgroundInactiveColor
		WRITE setGridSelectionBackgroundInactiveColor NOTIFY gridSelectionBackgroundInactiveColorChanged);

	Q_PROPERTY(QColor gridSelectionBorderColor READ gridSelectionBorderColor
		WRITE setGridSelectionBorderColor NOTIFY gridSelectionBorderColorChanged);

	Q_PROPERTY(QColor gridSelectionBorderInactiveColor READ gridSelectionBorderInactiveColor
		WRITE setGridSelectionBorderInactiveColor NOTIFY gridSelectionBorderColorInactiveChanged);

public:
	GridModel(QObject* parent = nullptr);

	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	void setModelDataAccessor(std::unique_ptr<IGridDataAccessor> accessor);

	const IGridDataAccessor* modelDataAcessor() const;
	IGridDataAccessor* modelDataAcessor();

	const IGridViewResizeStrategy* resizeStrategy() const;
	IGridViewResizeStrategy* resizeStrategy();

	QColor backgroundColor() const;
	Q_SLOT void setBackgroundColor(const QColor& color);
	Q_SIGNAL void backgroundColorChanged(const QColor&);

	QColor textColor() const;
	Q_SLOT void setTextColor(const QColor& color);
	Q_SIGNAL void textColorChanged(const QColor&);

	QColor headerBackgroundColor() const;
	Q_SLOT void setHeaderBackgroundColor(const QColor& color);
	Q_SIGNAL void headerBackgroundColorChanged(const QColor&);

	QColor headerTextColor() const;
	Q_SLOT void setHeaderTextColor(const QColor& color);
	Q_SIGNAL void headerTextColorChanged(const QColor&);

	QFont textFont() const;
	Q_SLOT void setTextFont(const QFont& font);
	Q_SIGNAL void textFontChanged(const QFont&);

	QColor gridSelectionBackgroundColor() const;
	Q_SLOT void setGridSelectionBackgroundColor(const QColor& color);
	Q_SIGNAL void gridSelectionBackgroundColorChanged(const QColor&);

	QColor gridSelectionBackgroundInactiveColor() const;
	Q_SLOT void setGridSelectionBackgroundInactiveColor(const QColor& color);
	Q_SIGNAL void gridSelectionBackgroundInactiveColorChanged(const QColor&);

	QColor gridSelectionBorderColor() const;
	Q_SLOT void setGridSelectionBorderColor(const QColor& color);
	Q_SIGNAL void gridSelectionBorderColorChanged(const QColor&);

	QColor gridSelectionBorderInactiveColor() const;
	Q_SLOT void setGridSelectionBorderInactiveColor(const QColor& color);
	Q_SIGNAL void gridSelectionBorderColorInactiveChanged(const QColor&);

signals:
	void modelDataAccessorChanged(IGridDataAccessor* accessor, IGridDataAccessor* oldAccessor);

private slots:
	void onRowAdded(int row);
	void onItemChanged(int row, int column);

private:
	// text color properties	
	QFont m_textFont;
	QColor m_textColor;

	// background color properties	
	QColor m_backgroundColor;

	// selection properties
	QColor m_gridSelectionBackgroundColor;
	QColor m_gridSelectionBackgroundInactiveColor;
	QColor m_gridSelectionBorderColor;
	QColor m_gridSelectionBorderInactiveColor;

	// header properties
	QColor m_headerTextColor;
	QColor m_headerBackgroundColor;

	std::unique_ptr<IGridDataAccessor> m_accessor;
};

}
