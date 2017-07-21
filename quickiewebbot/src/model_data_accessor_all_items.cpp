#include "application.h"
#include "model_data_accessor_all_items.h"
#include "service_locator.h"
#include "model_controller.h"
#include "grid_view_painter_text.h"
#include "grid_view_painter_background.h"

namespace QuickieWebBot
{

ModelDataAccessorAllItems::ModelDataAccessorAllItems(DataCollection::StorageType storageType)
	: m_modelControllerData(nullptr)
	, m_storageType(storageType)
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
		PageInfo::WordCountItemType
	};

	m_modelControllerData = theApp->modelController()->data();
	VERIFY(QObject::connect(m_modelControllerData, SIGNAL(pageInfoAdded(int, int)), this, SLOT(onModelDataRowAdded(int, int))));
}
	
int ModelDataAccessorAllItems::columnCount() const
{
	return m_columns.size();
}
	
QString ModelDataAccessorAllItems::columnText(int column) const
{
	return PageInfo::itemTitle(static_cast<PageInfo::ItemType>(m_columns[column]));
}
	
int ModelDataAccessorAllItems::rowCount() const
{
	return m_modelControllerData->guiStorage(m_storageType)->size();
}
	
QVariant ModelDataAccessorAllItems::itemValue(const QModelIndex& index) const
{
	const DataCollection::GuiStorageType* storage = m_modelControllerData->guiStorage(m_storageType);
	PageInfo::ItemType info = static_cast<PageInfo::ItemType>(m_columns[index.column()]);

	return PageInfo::itemValue((*storage)[index.row()], info);
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
	return m_columns[index.column()] == PageInfo::UrlItemType ? ItemFlagUrl : ItemFlagNone;
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

std::vector<GridViewPainter*> ModelDataAccessorAllItems::painters(const QModelIndex& index) const
{
	Q_UNUSED(index);

	static GridViewPainterText s_painterText;
	static GridViewPainterBackground s_painterBackground(Qt::transparent, Qt::transparent);

	return { &s_painterBackground, &s_painterText };
}


void ModelDataAccessorAllItems::onModelDataRowAdded(int row, int type)
{
	if (type == m_storageType)
	{
		emit rowAdded(row);
	}
}

}