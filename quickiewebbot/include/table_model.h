#pragma once

#include "model_data.h"

namespace QuickieWebBot
{

class TableModel 
	: public QAbstractTableModel
	, public ModelData
{
	Q_OBJECT

public:
	TableModel(QObject* parent = nullptr);
	TableModel(QVector<WebsiteAnalyseElement> const * const dataStorage, QObject* parent = nullptr);

	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const override;
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const override;
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const override;

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	virtual bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;
	virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;

	virtual bool insertColumns(int column, int count, QModelIndex const& parent = QModelIndex()) override;
	virtual bool removeColumns(int column, int count, QModelIndex const& parent = QModelIndex()) override;
};

}
