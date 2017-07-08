#include "model_data_accessor.h"
#include "page_info_item_accessor_helper.h"
#include "service_locator.h"
#include "model_controller.h"
#include "page_info_item_accessor_helper.h"

namespace QuickieWebBot
{

ModelDataAccessorAllItems::ModelDataAccessorAllItems(ModelControllerData::StorageType storageType)
	: m_modelControllerData(nullptr)
	, m_storageType(storageType)
{
	m_columns =
	{
		PageInfoItemAccessorHelper::Url,
		PageInfoItemAccessorHelper::Title,
		PageInfoItemAccessorHelper::TitleLength,
		PageInfoItemAccessorHelper::MetaRefresh,
		PageInfoItemAccessorHelper::MetaRobots,
		PageInfoItemAccessorHelper::RedirectedUrl,
		PageInfoItemAccessorHelper::ServerResponse,
		PageInfoItemAccessorHelper::MetaDescription,
		PageInfoItemAccessorHelper::MetaDescriptionLength,
		PageInfoItemAccessorHelper::MetaKeywords,
		PageInfoItemAccessorHelper::MetaKeywordsLength,
		PageInfoItemAccessorHelper::FirstH1,
		PageInfoItemAccessorHelper::FirstH1Length,
		PageInfoItemAccessorHelper::SecondH1,
		PageInfoItemAccessorHelper::SecondH1Length,
		PageInfoItemAccessorHelper::FirstH2,
		PageInfoItemAccessorHelper::FirstH2Length,
		PageInfoItemAccessorHelper::SecondH2,
		PageInfoItemAccessorHelper::SecondH2Length,
		PageInfoItemAccessorHelper::CanonicalLinkElement,
		PageInfoItemAccessorHelper::StatusCode,
		PageInfoItemAccessorHelper::PageSizeBytes,
		PageInfoItemAccessorHelper::WordCount
	};

	m_modelControllerData = ServiceLocator::instance()->service<ModelController>()->data();
	VERIFY(QObject::connect(m_modelControllerData, SIGNAL(pageInfoAdded(int, int)), this, SLOT(onModelDataRowAdded(int, int))));
}
	
int ModelDataAccessorAllItems::columnCount() const
{
	return static_cast<int>(m_columns.size());
}
	
QString ModelDataAccessorAllItems::columnText(int column) const
{
	return PageInfoItemAccessorHelper::title(static_cast<PageInfoItemAccessorHelper::ItemInfo>(m_columns[column]));
}
	
int ModelDataAccessorAllItems::rowCount() const
{
	return m_modelControllerData->guiStorage(m_storageType)->size();
}
	
QVariant ModelDataAccessorAllItems::itemValue(const QModelIndex& index) const
{
	const ModelControllerData::GuiStorageType* storage = m_modelControllerData->guiStorage(m_storageType);
	PageInfoItemAccessorHelper::ItemInfo info = static_cast<PageInfoItemAccessorHelper::ItemInfo>(m_columns[index.column()]);

	return PageInfoItemAccessorHelper::value((*storage)[index.row()], info);
}

QColor ModelDataAccessorAllItems::itemBackgroundColor(const QModelIndex& index) const
{
	return Qt::transparent;
}

int ModelDataAccessorAllItems::itemColSpan(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return 0;
}

int ModelDataAccessorAllItems::flags(const QModelIndex& index) const
{
	return m_columns[index.column()] == static_cast<int>(PageInfoItemAccessorHelper::Url) ? ItemFlagUrl : ItemFlagNone;
}

QAbstractItemDelegate* ModelDataAccessorAllItems::itemDelegate(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return nullptr;
}

QPixmap* ModelDataAccessorAllItems::pixmap(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return nullptr;
}

QObject* ModelDataAccessorAllItems::qobject()
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