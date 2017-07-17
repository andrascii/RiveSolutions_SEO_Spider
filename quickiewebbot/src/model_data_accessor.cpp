#include "application.h"
#include "model_data_accessor.h"
#include "page_info_item_accessor_helper.h"
#include "service_locator.h"
#include "model_controller.h"
#include "page_info_item_accessor_helper.h"
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
		PageInfoItemAccessorHelper::Url,
		PageInfoItemAccessorHelper::Title,
		PageInfoItemAccessorHelper::TitleLength,
		PageInfoItemAccessorHelper::MetaRefresh,
		PageInfoItemAccessorHelper::MetaRobots,
		PageInfoItemAccessorHelper::RedirectedUrl,
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

	m_modelControllerData = theApp->modelController()->data();
	VERIFY(QObject::connect(m_modelControllerData, SIGNAL(pageInfoAdded(int, int)), this, SLOT(onModelDataRowAdded(int, int))));
}
	
int ModelDataAccessorAllItems::columnCount() const
{
	return m_columns.size();
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
	const DataCollection::GuiStorageType* storage = m_modelControllerData->guiStorage(m_storageType);
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
	return m_columns[index.column()] == PageInfoItemAccessorHelper::Url ? ItemFlagUrl : ItemFlagNone;
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