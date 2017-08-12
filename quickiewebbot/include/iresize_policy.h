#pragma once

namespace QuickieWebBot
{

class TableView;

class IResizePolicy
{
public:
	virtual ~IResizePolicy() = default;

	virtual void init(TableView* gridView, const IResizePolicy* prev = nullptr) const noexcept = 0;

	virtual void resize(TableView* gridView) const noexcept = 0;
	virtual int columnSize(int column, const TableView* gridView) const noexcept = 0;
};

}