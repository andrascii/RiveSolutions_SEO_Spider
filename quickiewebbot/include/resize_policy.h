#pragma once

#include "iresize_policy.h"

namespace QuickieWebBot
{

class ResizePolicy : public IResizePolicy
{
public:
	virtual void init(TableView* view, const IResizePolicy* prev = nullptr)  const noexcept override;

	virtual void resize(TableView* view) const noexcept override;
	virtual int columnSize(int column, const TableView* view) const noexcept override;

	void setColumnsSize(std::map<int, int> size);

private:
	std::map<int, int> m_columnsSize;
};

}