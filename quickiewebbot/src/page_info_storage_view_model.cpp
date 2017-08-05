#include "page_info.h"
#include "igrid_model.h"
#include "grid_view_resize_policy.h"
#include "url_renderer.h"
#include "text_renderer.h"
#include "background_renderer.h"
#include "selection_background_renderer.h"
#include "page_info_storage_model.h"
#include "page_info_storage_view_model.h"
#include "quickie_web_bot_helpers.h"


namespace QuickieWebBot
{

PageInfoStorageViewModel::PageInfoStorageViewModel(PageInfoStorageModel* model, QObject* parent)
	: QObject(parent)
	, m_model(model)
	, m_resizePolicy(std::make_unique<GridViewResizePolicy>())
{
	//
	// columnPrefferedSize should not be in the PageInfo class
	//

	std::map<int, int> sizes =
	{
		{ PageInfo::UrlItemType, PageInfo::columnPrefferedSize(PageInfo::UrlItemType) },
		{ PageInfo::ContentItemType, PageInfo::columnPrefferedSize(PageInfo::ContentItemType) },
		{ PageInfo::TitleItemType, PageInfo::columnPrefferedSize(PageInfo::TitleItemType) },
		{ PageInfo::TitleLengthItemType, PageInfo::columnPrefferedSize(PageInfo::TitleLengthItemType) },
		{ PageInfo::MetaRefreshItemType, PageInfo::columnPrefferedSize(PageInfo::MetaRefreshItemType) },
		{ PageInfo::MetaRobotsItemType, PageInfo::columnPrefferedSize(PageInfo::MetaRobotsItemType) },
		{ PageInfo::MetaDescriptionItemType, PageInfo::columnPrefferedSize(PageInfo::MetaDescriptionItemType) },
		{ PageInfo::MetaKeywordsItemType, PageInfo::columnPrefferedSize(PageInfo::MetaKeywordsItemType) },
		{ PageInfo::RedirectedUrlItemType, PageInfo::columnPrefferedSize(PageInfo::RedirectedUrlItemType) },
		{ PageInfo::FirstH1ItemType, PageInfo::columnPrefferedSize(PageInfo::FirstH1ItemType) },
		{ PageInfo::SecondH1ItemType, PageInfo::columnPrefferedSize(PageInfo::SecondH1ItemType) },
		{ PageInfo::FirstH2ItemType, PageInfo::columnPrefferedSize(PageInfo::FirstH2ItemType) },
		{ PageInfo::SecondH2ItemType, PageInfo::columnPrefferedSize(PageInfo::SecondH2ItemType) },
		{ PageInfo::CanonicalLinkElementItemType, PageInfo::columnPrefferedSize(PageInfo::CanonicalLinkElementItemType) },
		{ PageInfo::StatusCodeItemType, PageInfo::columnPrefferedSize(PageInfo::StatusCodeItemType) },
		{ PageInfo::PageSizeKbItemType, PageInfo::columnPrefferedSize(PageInfo::PageSizeKbItemType) },
		{ PageInfo::WordCountItemType, PageInfo::columnPrefferedSize(PageInfo::WordCountItemType) },
		{ PageInfo::PageHashItemType, PageInfo::columnPrefferedSize(PageInfo::PageHashItemType) },
		{ PageInfo::UrlLengthItemType, PageInfo::columnPrefferedSize(PageInfo::UrlLengthItemType) },
		{ PageInfo::MetaDescriptionLengthItemType, PageInfo::columnPrefferedSize(PageInfo::MetaDescriptionLengthItemType) },
		{ PageInfo::MetaKeywordsLengthItemType, PageInfo::columnPrefferedSize(PageInfo::MetaKeywordsLengthItemType) },
		{ PageInfo::FirstH1LengthItemType, PageInfo::columnPrefferedSize(PageInfo::FirstH1LengthItemType) },
		{ PageInfo::SecondH1LengthItemType, PageInfo::columnPrefferedSize(PageInfo::SecondH1LengthItemType) },
		{ PageInfo::FirstH2LengthItemType, PageInfo::columnPrefferedSize(PageInfo::FirstH2LengthItemType) },
		{ PageInfo::SecondH2LengthItemType, PageInfo::columnPrefferedSize(PageInfo::SecondH2LengthItemType) },
		{ PageInfo::AltTextItemType, PageInfo::columnPrefferedSize(PageInfo::AltTextItemType) },
		{ PageInfo::AltTextLengthItemType, PageInfo::columnPrefferedSize(PageInfo::AltTextLengthItemType) },
		{ PageInfo::ImageSizeKbItemType, PageInfo::columnPrefferedSize(PageInfo::ImageSizeKbItemType) }
	};

	m_resizePolicy->setColumnsSize(sizes);
}

QList<IRenderer*> PageInfoStorageViewModel::renderers(const QModelIndex& index) const noexcept
{
	static TextRenderer s_textRenderer(std::pow(m_model->columnCount(index), 2));
	static UrlRenderer s_urlRenderer(std::pow(m_model->columnCount(index), 2));
	static SelectionBackgroundRenderer s_selectionBackgroundRenderer;
	static BackgroundRenderer s_backgroundRenderer;

	IRenderer* renderer = index.data(IGridModel::WhatsThisRole).toInt() == PageInfo::UrlItemType ? &s_urlRenderer : &s_textRenderer;

	return QList<IRenderer*>()
		<< &s_backgroundRenderer
		<< &s_selectionBackgroundRenderer
		<< renderer;
}

IGridViewResizePolicy* PageInfoStorageViewModel::resizePolicy() const noexcept
{
	return m_resizePolicy.get();
}

}