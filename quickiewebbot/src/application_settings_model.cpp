#include "application_settings_model.h"

namespace QuickieWebBot
{

ApplicationSettingsModel::ApplicationSettingsModel(QObject* parent)
	: QAbstractTableModel(parent)
{
	m_propGroupsNames.insert(PropGroups::CrawlerBehaviour, tr("Crawler Behaviour"));
	m_propGroupsNames.insert(PropGroups::CrawlerLimits, tr("Crawler Limits"));
	m_propGroupsNames.insert(PropGroups::Preferences, tr("Preferences"));
	m_propGroupsNames.insert(PropGroups::HTTPheader, tr("HTTP header"));
	m_propGroupsNames.insert(PropGroups::Proxy, tr("Proxy"));
	m_propGroupsNames.insert(PropGroups::Threads, tr("Threads"));
}

int ApplicationSettingsModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}

int ApplicationSettingsModel::rowCount(const QModelIndex& parent) const
{
	return m_propGroupsNames.size();
}

QVariant ApplicationSettingsModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole)
	{
		return m_propGroupsNames[static_cast<PropGroups>(index.row())];
	}

	//if (role == Qt::TextColorRole)
	//{
	//	return QColor(Qt::black);
	//}

	return QVariant();
}

QVariant ApplicationSettingsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

}