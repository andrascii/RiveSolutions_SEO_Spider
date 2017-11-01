#pragma once

namespace SeoSpider
{

class TableView;

class IResizePolicy
{
public:
	virtual ~IResizePolicy() = default;

	virtual void init(TableView* view, const IResizePolicy* prev = nullptr) const noexcept = 0;
	virtual int columnSize(int column, const TableView* view) const noexcept = 0;
	virtual void resize(TableView* view) const noexcept = 0;
	virtual QObject* qobject() noexcept = 0;

	// signals
	virtual void columnSizeChanged() = 0;
};

}