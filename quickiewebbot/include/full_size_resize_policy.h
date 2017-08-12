#pragma once
#include "iresize_policy.h"

namespace QuickieWebBot
{

class FullSizeResizePolicy : public IResizePolicy
{
public:
	FullSizeResizePolicy(std::vector<int> columnsPercentSize);
	virtual void resize(TableView* gridView) const noexcept override;
	virtual void init(TableView* gridView, const IResizePolicy* prev = nullptr) const noexcept override;
	virtual int columnSize(int column, const TableView* gridView) const noexcept override;

private:
	std::vector<int> m_columnsPercentSize;
};

}