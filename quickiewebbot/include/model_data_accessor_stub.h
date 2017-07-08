#pragma once

#include "imodel_data_accessor.h"
#include <QTimer>

namespace QuickieWebBot
{


class ModelDataAccessorItemStub 
	: public QObject
	, public ModelDataAccessorItemBase
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
		if (index.column() == 3)
		{
			return QVariant();
		}

		if (index.column() == 2)
		{
			return QUrl("http://google.com");
		}
		
		return QLatin1String("Item Text") + QString::number(index.row()) + QString::number(index.column());
	}

	virtual int flags(const QModelIndex& index) const override
	{
		return index.column() == 2 ? IModelDataAccessorItem::ItemFlagUrl : IModelDataAccessorItem::ItemFlagNone;
	}

	virtual int itemColSpan(const QModelIndex& index) const override
	{
		if (index.column() == 2)
		{
			return 2;
		}

		return 0;
	}

	virtual QAbstractItemDelegate* itemDelegate(const QModelIndex& index) const override
	{
		Q_UNUSED(index);
		return nullptr;
	}

	virtual QObject* qobject() 
	{
		return this;
	}

	virtual QPixmap* pixmap(const QModelIndex& index) const override
	{
		static QPixmap s_pixmap = QPixmap(":/images/arrow-down-icon.png");

		if (index.column() == 0) 
		{
			return &s_pixmap;
		}
		
		return nullptr;
	}

	virtual QColor itemBackgroundColor(const QModelIndex& index) const override
	{
		return index.row() % 2 == 0 ? Qt::transparent : Qt::lightGray;
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