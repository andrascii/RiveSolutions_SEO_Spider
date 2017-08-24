#pragma once

namespace QuickieWebBot
{

class ApplicationSettingsModel : public QAbstractTableModel
{
	Q_OBJECT

private:

	enum class PropGroups
	{
		CrawlerBehaviour,
		CrawlerLimits,
		Preferences,
		HTTPheader,
		Proxy,
		Threads
	};

	QMap<PropGroups, QString> m_propGroupsNames;

public:
	ApplicationSettingsModel(QObject* parent = nullptr);

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	//virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

};

}