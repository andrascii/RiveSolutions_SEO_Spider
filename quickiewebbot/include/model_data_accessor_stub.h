#pragma once

#include "imodel_data_accessor.h"
#include <QTimer>

namespace QuickieWebBot
{


class ModelDataAccessorItemStub 
	: public QObject
	, public IModelDataAccessorItem
{
	Q_OBJECT

public:
	ModelDataAccessorItemStub() 
	{
		m_rows = 5;
		
		QTimer* timer = new QTimer(this);
		VERIFY(QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timeOut())));
		timer->start(1000);
	}

	virtual int columnCount() const override
	{
		return 10;
	}

	virtual int rowCount() const override
	{
		return m_rows;
	}

	virtual QString columnText(int column) const override
	{
		return QLatin1String("Column Name") + QString::number(column);
	}

	virtual QVariant itemValue(const QModelIndex& index) const override
	{
		return QLatin1String("Item Text") + QString::number(index.row()) + QString::number(index.column());
	}

	virtual int itemColSpan(const QModelIndex& index) const override
	{
		return 0;
	}

	virtual QAbstractItemDelegate* itemDelegate(const QModelIndex& index) const override
	{
		return nullptr;
	}

	virtual QObject* qobject() 
	{
		return this;
	}

	// signals
	Q_SIGNAL void itemChanged(QModelIndex index);
	Q_SIGNAL void rowRemoved(int row);
	Q_SIGNAL void rowAdded(int row);
	Q_SIGNAL void reset();

private:
	Q_SLOT void timeOut()
	{
		m_rows++;
		emit rowAdded(m_rows);
	}

	int m_rows;
};

class ModelDataAccessorStub : public IModelDataAccessor
{
public:

	virtual std::unique_ptr<IModelDataAccessorItem> allProcessedItems() const override
	{
		return std::make_unique<ModelDataAccessorItemStub>();
	}

};

}