#include "model_data.h"

namespace QuickieWebBot
{

ModelData::ModelData(QVector<WebsiteAnalyseElement> const * const dataStorage)
	: m_dataStorage(dataStorage)
{
}

void ModelData::setDataStorage(QVector<WebsiteAnalyseElement> const * const dataStorage) noexcept
{
	m_dataStorage = dataStorage;
}

void ModelData::setHeaderItems(std::initializer_list<QString> lst) noexcept
{
	for (QString const& item : lst)
	{
		m_headerItems.push_back(item);
	}
}

QVector<QuickieWebBot::WebsiteAnalyseElement> const& ModelData::dataStorage() const noexcept
{
	return *m_dataStorage;
}

QVector<QString> const& ModelData::headerItems() const noexcept
{
	return m_headerItems;
}

}