#include "application.h"
#include "grid_data_accessor_all_items.h"
#include "service_locator.h"
#include "model_controller.h"
#include "grid_view_painter_text.h"
#include "grid_view_painter_background.h"
#include "grid_view_resize_strategy.h"
#include "quickie_web_bot_helpers.h"
#include "grid_data_accessor_factory.h"

namespace QuickieWebBot
{

GridDataAccessorAllItems::GridDataAccessorAllItems(DataCollection::StorageType storageType, QVector<PageInfo::ItemType> columns)
	: m_modelControllerData(nullptr)
	, m_storageType(storageType)
	, m_columns(columns)
	, m_resizeStrategy(std::make_unique<GridViewResizeStrategy>())
	, m_painterBackground(Qt::transparent, Qt::transparent)
{
	if (m_columns.size() == 0)
	{
		m_columns =
		{
			PageInfo::UrlItemType,
			PageInfo::ContentItemType,
			PageInfo::TitleItemType,
			PageInfo::TitleLengthItemType,
			PageInfo::MetaRefreshItemType,
			PageInfo::MetaRobotsItemType,
			PageInfo::RedirectedUrlItemType,
			PageInfo::MetaDescriptionItemType,
			PageInfo::MetaDescriptionLengthItemType,
			PageInfo::MetaKeywordsItemType,
			PageInfo::MetaKeywordsLengthItemType,
			PageInfo::FirstH1ItemType,
			PageInfo::FirstH1LengthItemType,
			PageInfo::SecondH1ItemType,
			PageInfo::SecondH1LengthItemType,
			PageInfo::FirstH2ItemType,
			PageInfo::FirstH2LengthItemType,
			PageInfo::SecondH2ItemType,
			PageInfo::SecondH2LengthItemType,
			PageInfo::CanonicalLinkElementItemType,
			PageInfo::StatusCodeItemType,
			PageInfo::PageSizeKbItemType,
			PageInfo::WordCountItemType,
			PageInfo::PageHashItemType
		};
	}

	m_modelControllerData = theApp->modelController()->data();
	VERIFY(QObject::connect(m_modelControllerData, SIGNAL(pageInfoAdded(int, int)), this, SLOT(onModelDataRowAdded(int, int))));

	int columnIndex = 0;
	std::map<int, int> columnsSize;

	for (PageInfo::ItemType column : m_columns)
	{
		columnsSize[columnIndex] = PageInfo::columnPrefferedSize(column);
		columnIndex++;
	}

	m_resizeStrategy->setColumnsSize(columnsSize);
}
	
int GridDataAccessorAllItems::columnsCount() const
{
	return m_columns.size();
}

int GridDataAccessorAllItems::itemCount() const
{
	return m_modelControllerData->guiStorage(m_storageType)->size();
}
	
QVariant GridDataAccessorAllItems::columnDescription(int section) const
{
	if (section >= m_columns.size())
	{
		return QVariant();
	}

	return PageInfo::itemTitle(m_columns[section]);
}

QVariant GridDataAccessorAllItems::item(const QModelIndex& index) const
{
	return item(index.row(), index.column());
}

QVariant GridDataAccessorAllItems::item(int row, int column) const
{
	const DataCollection::GuiStorageType& storage = *m_modelControllerData->guiStorage(m_storageType);

	PageInfo::ItemType info = m_columns[column];

	return storage[row]->itemValue(info);
}

PageInfoPtr GridDataAccessorAllItems::pageInfoAtRow(int row) const
{
	const DataCollection::GuiStorageType& storage = *m_modelControllerData->guiStorage(m_storageType);

	return storage[row];
}

IGridViewResizeStrategy* GridDataAccessorAllItems::resizeStrategy() const
{
	return m_resizeStrategy.get();
}

GridDataAccessorFactoryParams GridDataAccessorAllItems::childViewParams(const QItemSelection& selection) const
{
	QModelIndexList indicies = selection.indexes();

	if (indicies.isEmpty())
	{
		return GridDataAccessorFactoryParams(GridDataAccessorFactoryParams::TypeInvalid, GridDataAccessorFactoryParams::ModeGeneral);
	}

	QModelIndex index = indicies.first();
	assert(index.isValid());

	return GridDataAccessorFactoryParams(static_cast<GridDataAccessorFactoryParams::Type>(m_storageType), 
		GridDataAccessorFactoryParams::ModeOneRow, index.row());
}

QObject* GridDataAccessorAllItems::qobject()
{
	return this;
}

void GridDataAccessorAllItems::onModelDataRowAdded(int row, int type)
{
	onModelDataRowAddedInternal(row, type);
}

void GridDataAccessorAllItems::onModelDataRowAddedInternal(int row, int type)
{
	if (type == m_storageType)
	{
		emit rowAdded(row);
	}
}

}