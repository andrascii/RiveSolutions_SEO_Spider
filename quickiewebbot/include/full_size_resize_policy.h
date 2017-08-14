#pragma once
#include "iresize_policy.h"

namespace QuickieWebBot
{

class FullSizeResizePolicy : public QObject, public IResizePolicy
{
	Q_OBJECT

public:
	FullSizeResizePolicy(std::vector<int> columnsPercentSize);

public:
	// IResizePolicy implementation

	virtual void resize(TableView* gridView) const noexcept override;
	virtual void init(TableView* gridView, const IResizePolicy* prev = nullptr) const noexcept override;
	virtual int columnSize(int column, const TableView* gridView) const noexcept override;
	virtual QObject* qobject() noexcept override;

	Q_SIGNAL virtual void columnSizeChanged() override;

private:
	std::vector<int> m_columnsPercentSize;
};

}