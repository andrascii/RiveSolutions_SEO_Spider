#pragma once

namespace SeoSpider
{

class TableProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:
	explicit TableProxyModel(QObject* parent = nullptr);
	void setAcceptedResources(int resources);
	int acceptedResources() const;
protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const  override;

private:
	int m_acceptedResources;
};

}