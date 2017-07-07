#include "model_data_accessor.h"
#include "website_analyse_element_info.h"
#include "service_locator.h"
#include "model_controller.h"

namespace QuickieWebBot
{

ModelDataAccessorAllItems::ModelDataAccessorAllItems(ModelControllerData::StorageType storageType)
	: m_modelControllerData(nullptr)
	, m_storageType(storageType)
{
	m_columns =
	{
		WebsiteAnalyseElementInfo::Url,
		WebsiteAnalyseElementInfo::Title,
		WebsiteAnalyseElementInfo::TitleLength,
		WebsiteAnalyseElementInfo::MetaRefresh,
		WebsiteAnalyseElementInfo::MetaRobots,
		WebsiteAnalyseElementInfo::RedirectedUrl,
		WebsiteAnalyseElementInfo::ServerResponse,
		WebsiteAnalyseElementInfo::MetaDescription,
		WebsiteAnalyseElementInfo::MetaDescriptionLength,
		WebsiteAnalyseElementInfo::MetaKeywords,
		WebsiteAnalyseElementInfo::MetaKeywordsLength,
		WebsiteAnalyseElementInfo::FirstH1,
		WebsiteAnalyseElementInfo::FirstH1Length,
		WebsiteAnalyseElementInfo::SecondH1,
		WebsiteAnalyseElementInfo::SecondH1Length,
		WebsiteAnalyseElementInfo::FirstH2,
		WebsiteAnalyseElementInfo::FirstH2Length,
		WebsiteAnalyseElementInfo::SecondH2,
		WebsiteAnalyseElementInfo::SecondH2Length,
		WebsiteAnalyseElementInfo::CanonicalLinkElement,
		WebsiteAnalyseElementInfo::StatusCode,
		WebsiteAnalyseElementInfo::PageSizeBytes,
		WebsiteAnalyseElementInfo::WordCount
	};

	m_modelControllerData = ServiceLocator::instance()->service<ModelController>();
	VERIFY(QObject::connect(m_modelControllerData, SIGNAL(rowAdded(int, int)), this, SLOT(onModelDataRowAdded(int, int))));
}
	
int ModelDataAccessorAllItems::columnCount() const
{
	return static_cast<int>(m_columns.size());
}
	
QString ModelDataAccessorAllItems::columnText(int column) const
{
	return WebsiteAnalyseElementInfo::title(static_cast<WebsiteAnalyseElementInfo::ElementInfo>(m_columns[column]));
}
	
int ModelDataAccessorAllItems::rowCount() const
{
	return m_modelControllerData->guiStorage(m_storageType)->size();
}
	
QVariant ModelDataAccessorAllItems::itemValue(const QModelIndex& index) const
{
	const auto storage = m_modelControllerData->guiStorage(m_storageType);
	auto info = static_cast<WebsiteAnalyseElementInfo::ElementInfo>(m_columns[index.column()]);
	return WebsiteAnalyseElementInfo::value((*storage)[index.row()], info);
}

QColor ModelDataAccessorAllItems::itemBackgroundColor(const QModelIndex & index) const
{
	return Qt::transparent;
}

int ModelDataAccessorAllItems::itemColSpan(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return 0;
}

QAbstractItemDelegate * ModelDataAccessorAllItems::itemDelegate(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return nullptr;
}

QPixmap * ModelDataAccessorAllItems::pixmap(const QModelIndex& index) const
{
	Q_UNUSED(index);
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