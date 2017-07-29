#include "application.h"
#include "model_data_accessor_all_items.h"
#include "service_locator.h"
#include "model_controller.h"
#include "grid_view_painter_text.h"
#include "grid_view_painter_background.h"
#include "grid_view_resize_strategy.h"
#include "quickie_web_bot_helpers.h"
#include "model_data_accessor_factory.h"

namespace QuickieWebBot
{

ModelDataAccessorAllItems::ModelDataAccessorAllItems(DataCollection::StorageType storageType, QVector<PageInfo::ItemType> columns)
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

int ModelDataAccessorAllItems::columnCount() const
{
	return m_columns.size();
}
	
QString ModelDataAccessorAllItems::headerData(int column, Qt::Orientation orientation) const
{
	if (orientation == Qt::Horizontal)
	{
		assert(column < m_columns.size());
		return PageInfo::itemTitle(m_columns[column]);
	}

	return QString("%1").arg(column);
}
	
int ModelDataAccessorAllItems::rowCount() const
{
	return m_modelControllerData->guiStorage(m_storageType)->size();
}
	
QVariant ModelDataAccessorAllItems::itemValue(const QModelIndex& index) const
{
	return itemValue(index.row(), index.column());
}

QVariant ModelDataAccessorAllItems::itemValue(int row, int column) const
{
	const DataCollection::GuiStorageType& storage = *m_modelControllerData->guiStorage(m_storageType);

	PageInfo::ItemType info = m_columns[column];

	return storage[row]->itemValue(info);
}

PageInfoPtr ModelDataAccessorAllItems::pageInfoAtRow(int row) const
{
	const DataCollection::GuiStorageType& storage = *m_modelControllerData->guiStorage(m_storageType);

	return storage[row];
}

QColor ModelDataAccessorAllItems::itemBackgroundColor(const QModelIndex& index) const
{
	return Qt::transparent;
}

int ModelDataAccessorAllItems::itemColSpan(const QModelIndex& index) const
{
	return 1;
}

int ModelDataAccessorAllItems::flags(const QModelIndex& index) const
{
	return m_columns[index.column()] == PageInfo::UrlItemType ? ItemFlagUrl : ItemFlagNone;
}

QPixmap* ModelDataAccessorAllItems::pixmap(const QModelIndex& index) const
{
	return nullptr;
}

QObject* ModelDataAccessorAllItems::qobject()
{
	return this;
}

std::vector<const GridViewPainter*> ModelDataAccessorAllItems::painters(const QModelIndex& index) const
{
	return { &m_painterBackground, &m_painterText };
}

std::vector<GridViewPainter*> ModelDataAccessorAllItems::painters(const QModelIndex& index)
{
	return { &m_painterBackground, &m_painterText };
}

IGridViewResizeStrategy* ModelDataAccessorAllItems::resizeStrategy() const
{
	return m_resizeStrategy.get();
}

ModelDataAccessorFactoryParams ModelDataAccessorAllItems::childViewParams(const QItemSelection& selection) const
{
	QModelIndexList indicies = selection.indexes();

	if (indicies.isEmpty())
	{
		return ModelDataAccessorFactoryParams(ModelDataAccessorFactoryParams::TypeInvalid, ModelDataAccessorFactoryParams::ModeGeneral);
	}

	QModelIndex index = indicies.first();
	assert(index.isValid());

	return ModelDataAccessorFactoryParams(static_cast<ModelDataAccessorFactoryParams::Type>(m_storageType), 
		ModelDataAccessorFactoryParams::ModeOneRow, index.row());
}

const GridViewPainter* ModelDataAccessorAllItems::backgroundPainter(const QModelIndex& index) const
{
	return &m_painterBackground;
}


GridViewPainter* ModelDataAccessorAllItems::backgroundPainter(const QModelIndex& index)
{
	return &m_painterBackground;
}

const GridViewPainter* ModelDataAccessorAllItems::textPainter(const QModelIndex& index) const
{
	return &m_painterText;
}


GridViewPainter* ModelDataAccessorAllItems::textPainter(const QModelIndex& index)
{
	return &m_painterText;
}

void ModelDataAccessorAllItems::onModelDataRowAdded(int row, int type)
{
	onModelDataRowAddedInternal(row, type);
}

void ModelDataAccessorAllItems::onModelDataRowAddedInternal(int row, int type)
{
	if (type == m_storageType)
	{
		emit rowAdded(row);
	}
}

}