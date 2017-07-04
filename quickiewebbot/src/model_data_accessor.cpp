#include "model_data_accessor.h"
#include "website_analyse_element_info.h"

namespace QuickieWebBot
{

ModelDataAccessorAllItems::ModelDataAccessorAllItems(ModelControllerData* data, ModelControllerData::StorageType storageType)
	: m_modelControllerData(data)
	, m_storageType(storageType)
	, m_columns{ WebsiteAnalyseElementInfo::Url, WebsiteAnalyseElementInfo::Title }
{
	VERIFY(QObject::connect(data, SIGNAL(rowAdded(int, int)), this, SLOT(onModelDataRowAdded(int, int))));
}
	
int ModelDataAccessorAllItems::columnCount() const
{
	return static_cast<int>(m_columns.size());
}
	
QString ModelDataAccessorAllItems::columnText(int column) const
{
	return WebsiteAnalyseElementInfo::getTitle(static_cast<WebsiteAnalyseElementInfo::Info>(m_columns[column]));
}
	
int ModelDataAccessorAllItems::rowCount() const
{
	return m_modelControllerData->guiStorage(m_storageType)->size();
}
	
QVariant ModelDataAccessorAllItems::itemValue(const QModelIndex& index) const
{
	const auto storage = m_modelControllerData->guiStorage(m_storageType);
	auto info = static_cast<WebsiteAnalyseElementInfo::Info>(m_columns[index.column()]);
	return WebsiteAnalyseElementInfo::getValue((*storage)[index.row()], info);
}

int ModelDataAccessorAllItems::itemColSpan(const QModelIndex & index) const
{
	return 0;
}

QAbstractItemDelegate * ModelDataAccessorAllItems::itemDelegate(const QModelIndex & index) const
{
	return nullptr;
}

QObject * ModelDataAccessorAllItems::qobject()
{
	return this;
}

void ModelDataAccessorAllItems::onModelDataRowAdded(int row, int type)
{
	if (type == m_storageType)
	{
		emit rowAdded(row);
	}
}

}