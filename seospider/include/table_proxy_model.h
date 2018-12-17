#pragma once

namespace SeoSpider
{

class TableSortFilterProxyModel : public QSortFilterProxyModel
{
public:
	explicit TableSortFilterProxyModel(QObject* parent = nullptr);
protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const  override;

private:
	int m_acceptedResources;
};

}