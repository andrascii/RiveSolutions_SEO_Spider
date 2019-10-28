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
	void setAcceptedCanonicalUrl(const QString& acceptedCanonicalUrl);
protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const  override;

private:
	int m_acceptedResources;
	QString m_acceptedCanonicalUrl;
};

}