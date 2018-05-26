#pragma once
#include "iresize_policy.h"

namespace SeoSpider
{

class ViewportPercentResizePolicy : public QObject, public IResizePolicy
{
	Q_OBJECT

public:
	ViewportPercentResizePolicy(QVector<int> columnsPercentSize);

public:
	// IResizePolicy implementation

	virtual void resize(TableView* view) const noexcept override;
	virtual void init(TableView* view, const IResizePolicy* prev = nullptr) const noexcept override;
	virtual int columnSize(int column, const TableView* view) const noexcept override;
	virtual QObject* qobject() noexcept override;

	Q_SIGNAL virtual void columnSizeChanged() override;

private:
	QVector<int> m_columnsPercentSize;
};

}