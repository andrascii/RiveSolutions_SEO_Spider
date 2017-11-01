#pragma once

#include "iresize_policy.h"

namespace SeoSpider
{

class DefaultColumnResizePolicy : public QObject, public IResizePolicy
{
	Q_OBJECT

public:
	void setColumnsSize(const std::map<int, int>& size);

public: 
	// IResizePolicy implementation
	
	virtual void init(TableView* view, const IResizePolicy* prev = nullptr) const noexcept override;
	virtual int columnSize(int column, const TableView* view) const noexcept override;
	virtual void resize(TableView* view) const noexcept override;
	virtual QObject* qobject() noexcept override;

	Q_SIGNAL virtual void columnSizeChanged() override;

private:
	std::map<int, int> m_columnsSize;
};

}