#pragma once
#include "iresize_policy.h"

namespace QuickieWebBot
{

class ViewportPercentResizePolicy : public QObject, public IResizePolicy
{
	Q_OBJECT

public:
	ViewportPercentResizePolicy(std::vector<int> columnsPercentSize);

public:
	// IResizePolicy implementation

	virtual void resize(TableView* view) const noexcept override;
	virtual void init(TableView* view, const IResizePolicy* prev = nullptr) const noexcept override;
	virtual int columnSize(int column, const TableView* view) const noexcept override;
	virtual QObject* qobject() noexcept override;

	Q_SIGNAL virtual void columnSizeChanged() override;

private:
	std::vector<int> m_columnsPercentSize;
};

}