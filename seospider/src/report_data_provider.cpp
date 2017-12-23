#include "report_data_provider.h"
#include "seo_spider_helpers.h"
#include "application.h"
#include "crawler.h"
#include "irobots_txt_loader.h"
#include "isequenced_storage.h"
#include "storage_type.h"

namespace
{

using namespace SeoSpider;

const QMap<ReportDataKeys, QByteArray> s_placeHolders
{
	// Indexing and Page Scanning
	{ ReportDataKeys::StatusCode4xx, "stat_code_4xx" },
	{ ReportDataKeys::StatusCode4xxImage, "stat_code_4xx_image" },
	{ ReportDataKeys::StatusCode4xxCount, "stat_code_4xx_count" },
	{ ReportDataKeys::StatusCode5xx, "stat_code_5xx" },
	{ ReportDataKeys::StatusCode5xxImage, "stat_code_5xx_image" },
	{ ReportDataKeys::StatusCode5xxCount, "stat_code_5xx_count" },
	{ ReportDataKeys::NotIndexedPages, "not_indexed_pages" },
	{ ReportDataKeys::NotIndexedPagesImage, "not_indexed_pages_image" },
	{ ReportDataKeys::NotIndexedPagesCount, "not_indexed_pages_count" },
	{ ReportDataKeys::ConfiguredCorrectly404, "404_configured_correctly" },
	{ ReportDataKeys::ConfiguredCorrectly404Image, "404_configured_correctly_image" },
	{ ReportDataKeys::ConfiguredCorrectly404Count, "404_configured_correctly_count" },
	{ ReportDataKeys::RobotsTxt, "robotstxt" },
	{ ReportDataKeys::RobotsTxtImage, "robotstxt_image" },
	{ ReportDataKeys::RobotsTxtCount, "robotstxt_count" },
	{ ReportDataKeys::XmlSitemap, "xml_sitemap" },
	{ ReportDataKeys::XmlSitemapImage, "xml_sitemap_image" },
	{ ReportDataKeys::XmlSitemapCount, "xml_sitemap_count" },

	// Redirections
	{ ReportDataKeys::WwwFixedVersion, "www_fixed_version" },
	{ ReportDataKeys::WwwFixedVersionImage, "www_fixed_version_image" },
	{ ReportDataKeys::WwwFixedVersionCount, "www_fixed_version_count" },
	{ ReportDataKeys::Redirections302, "302_redirections" },
	{ ReportDataKeys::Redirections302Image, "302_redirections_image" },
	{ ReportDataKeys::Redirections302Count, "302_redirections_count" },
	{ ReportDataKeys::Redirections301, "301_redirections" },
	{ ReportDataKeys::Redirections301Image, "301_redirections_image" },
	{ ReportDataKeys::Redirections301Count, "301_redirections_count" },
	{ ReportDataKeys::LargeAmountRedirects, "large_amount_redirects" },
	{ ReportDataKeys::LargeAmountRedirectsImage, "large_amount_redirects_image" },
	{ ReportDataKeys::LargeAmountRedirectsCount, "large_amount_redirects_count" },
	{ ReportDataKeys::RefreshMetaTag, "refresh_meta_tag" },
	{ ReportDataKeys::RefreshMetaTagImage, "refresh_meta_tag_image" },
	{ ReportDataKeys::RefreshMetaTagCount, "refresh_meta_tag_count" },
	{ ReportDataKeys::RelCanonicalPages, "rel_canonical_pages" },
	{ ReportDataKeys::RelCanonicalPagesImage, "rel_canonical_pages_image" },
	{ ReportDataKeys::RelCanonicalPagesCount, "rel_canonical_pages_count" },

	// Links
	{ ReportDataKeys::BrokenLinks, "broken_links" },
	{ ReportDataKeys::BrokenLinksImage, "broken_links_image" },
	{ ReportDataKeys::BrokenLinksCount, "broken_links_count" },
	{ ReportDataKeys::PagesWithLargeAmountOfLinks, "large_amount_links" },
	{ ReportDataKeys::PagesWithLargeAmountOfLinksImage, "large_amount_links_image" },
	{ ReportDataKeys::PagesWithLargeAmountOfLinksCount, "large_amount_links_count" },
	{ ReportDataKeys::ExternalDofollowLinks, "ext_dofollow_links" },
	{ ReportDataKeys::ExternalDofollowLinksImage, "ext_dofollow_links_image" },
	{ ReportDataKeys::ExternalDofollowLinksCount, "ext_dofollow_links_count" },
	{ ReportDataKeys::TooLongUrlAddresses, "too_long_url" },
	{ ReportDataKeys::TooLongUrlAddressesImage, "too_long_url_image" },
	{ ReportDataKeys::TooLongUrlAddressesCount, "too_long_url_count" },

	// Technical factors
	{ ReportDataKeys::PagesWithDuplicatedRelCanonical, "duplicated_rel_canonical" },
	{ ReportDataKeys::PagesWithDuplicatedRelCanonicalImage, "duplicated_rel_canonical_image" },
	{ ReportDataKeys::PagesWithDuplicatedRelCanonicalCount, "duplicated_rel_canonical_count" },
	{ ReportDataKeys::PagesContainsFrames, "pages_contains_frames" },
	{ ReportDataKeys::PagesContainsFramesImage, "pages_contains_frames_image" },
	{ ReportDataKeys::PagesContainsFramesCount, "pages_contains_frames_count" },
	{ ReportDataKeys::PagesWithHtmlErrors, "pages_error_html" },
	{ ReportDataKeys::PagesWithHtmlErrorsImage, "pages_error_html_image" },
	{ ReportDataKeys::PagesWithHtmlErrorsCount, "pages_error_html_count" },
	{ ReportDataKeys::PagesWithCssErrorsWarnings, "pages_error_css" },
	{ ReportDataKeys::PagesWithCssErrorsWarningsImage, "pages_error_css_image" },
	{ ReportDataKeys::PagesWithCssErrorsWarningsCount, "pages_error_css_count" },
	{ ReportDataKeys::TooLargePages, "too_large_pages" },
	{ ReportDataKeys::TooLargePagesImage, "too_large_pages_image" },
	{ ReportDataKeys::TooLargePagesCount, "too_large_pages_count" },

	// Images
	{ ReportDataKeys::BrokenImages, "broken_images" },
	{ ReportDataKeys::BrokenImagesImage, "broken_images_image" },
	{ ReportDataKeys::BrokenImagesCount, "broken_images_count" },
	{ ReportDataKeys::ImagesWithEmptyAltText, "img_empty_alt" },
	{ ReportDataKeys::ImagesWithEmptyAltTextImage, "img_empty_alt_image" },
	{ ReportDataKeys::ImagesWithEmptyAltTextCount, "img_empty_alt_count" },

	// On Page
	{ ReportDataKeys::EmptyTitles, "empty_titles" },
	{ ReportDataKeys::EmptyTitlesImage, "empty_titles_image" },
	{ ReportDataKeys::EmptyTitlesCount, "empty_titles_count" },
	{ ReportDataKeys::DuplicatedTitles, "duplicated_titles" },
	{ ReportDataKeys::DuplicatedTitlesImage, "duplicated_titles_image" },
	{ ReportDataKeys::DuplicatedTitlesCount, "duplicated_titles_count" },
	{ ReportDataKeys::TooLongTitles, "too_long_titles" },
	{ ReportDataKeys::TooLongTitlesImage, "too_long_titles_image" },
	{ ReportDataKeys::TooLongTitlesCount, "too_long_titles_count" },
	{ ReportDataKeys::EmptyMetaDescriptions, "empty_meta_descriptions" },
	{ ReportDataKeys::EmptyMetaDescriptionsImage, "empty_meta_descriptions_image" },
	{ ReportDataKeys::EmptyMetaDescriptionsCount, "empty_meta_descriptions_count" },
	{ ReportDataKeys::DuplicatedMetaDescriptions, "duplicated_meta_descriptions" },
	{ ReportDataKeys::DuplicatedMetaDescriptionsImage, "duplicated_meta_descriptions_image" },
	{ ReportDataKeys::DuplicatedMetaDescriptionsCount, "duplicated_meta_descriptions_count" },
	{ ReportDataKeys::TooLongMetaDescriptions, "too_long_meta_descriptions" },
	{ ReportDataKeys::TooLongMetaDescriptionsImage, "too_long_meta_descriptions_image" },
	{ ReportDataKeys::TooLongMetaDescriptionsCount, "too_long_meta_descriptions_count" },
};

}

namespace SeoSpider
{

ReportDataProvider::ReportDataProvider(CrawlerEngine::SequencedDataCollection* sequencedDataCollection)
	: m_sequencedDataCollection(sequencedDataCollection)
{
	QVector<QString> paths
	{
		{ QStringLiteral(":/images/icon-ok.svg") },
		{ QStringLiteral(":/images/icon-warning.svg") },
		{ QStringLiteral(":/images/icon-error.svg") },
	};

	m_pixmaps[ItemStatus::StatusOK] = QPixmap(SeoSpiderHelpers::pointsToPixels(13.5), SeoSpiderHelpers::pointsToPixels(13.5));
	m_pixmaps[ItemStatus::StatusWarning] = QPixmap(SeoSpiderHelpers::pointsToPixels(13.5), SeoSpiderHelpers::pointsToPixels(13.5));
	m_pixmaps[ItemStatus::StatusError] = QPixmap(SeoSpiderHelpers::pointsToPixels(13.5), SeoSpiderHelpers::pointsToPixels(13.5));

	QSvgRenderer svgRenderer;

	for (int i = 0; i < paths.size(); ++i)
	{
		m_pixmaps[static_cast<ItemStatus>(i)].fill(Qt::transparent);
		QPainter painterPixmap(&m_pixmaps[static_cast<ItemStatus>(i)]);

		svgRenderer.load(paths[i]);
		svgRenderer.render(&painterPixmap);
	}
}

QVariant ReportDataProvider::data(ReportDataKeys dataKey) const
{
	switch (dataKey)
	{
		// Indexing and Page Scanning
		case ReportDataKeys::StatusCode4xx:
		{
			return QObject::tr("Resources with status code 4xx");
		}
		case ReportDataKeys::StatusCode4xxImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::StatusCode4xxCount:
		{
			return 0;
		}
		case ReportDataKeys::StatusCode5xx:
		{
			return QObject::tr("Resources with status code 5xx");
		}
		case ReportDataKeys::StatusCode5xxImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::StatusCode5xxCount:
		{
			return 0;
		}
		case ReportDataKeys::NotIndexedPages:
		{
			return QObject::tr("Not indexed pages");
		}
		case ReportDataKeys::NotIndexedPagesImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::NotIndexedPagesCount:
		{
			return 0;
		}
		case ReportDataKeys::ConfiguredCorrectly404:
		{
			return QObject::tr("404 page configured correctly");
		}
		case ReportDataKeys::ConfiguredCorrectly404Image:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::ConfiguredCorrectly404Count:
		{
			return QObject::tr("TO DO check validness");
		}
		case ReportDataKeys::RobotsTxt:
		{
			return QObject::tr("Has robots.txt file");
		}
		case ReportDataKeys::RobotsTxtImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::RobotsTxtCount:
		{
			CrawlerEngine::Crawler* crawler = theApp->crawler();

			return crawler->robotsTxtLoader()->isValid() ? 
				QObject::tr("Yes") : 
				QObject::tr("No");
		}
		case ReportDataKeys::XmlSitemap:
		{
			return QObject::tr("Has .xml sitemap");
		}
		case ReportDataKeys::XmlSitemapImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::XmlSitemapCount:
		{
			return QObject::tr("TO DO checking existness");
		}

		// Redirections
		case ReportDataKeys::WwwFixedVersion:
		{
			return QObject::tr("Fixed www version and without www");
		}
		case ReportDataKeys::WwwFixedVersionImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::WwwFixedVersionCount:
		{
			return QObject::tr("TO DO check validness");
		}
		case ReportDataKeys::Redirections302:
		{
			return QObject::tr("302 redirection pages");
		}
		case ReportDataKeys::Redirections302Image:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::Redirections302Count:
		{
			return 0;
		}
		case ReportDataKeys::Redirections301:
		{
			return QObject::tr("301 redirection pages");
		}
		case ReportDataKeys::Redirections301Image:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::Redirections301Count:
		{
			return 0;
		}
		case ReportDataKeys::LargeAmountRedirects:
		{
			return QObject::tr("Pages with large amount of redirects");
		}
		case ReportDataKeys::LargeAmountRedirectsImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::LargeAmountRedirectsCount:
		{
			return 0;
		}
		case ReportDataKeys::RefreshMetaTag:
		{
			return QObject::tr("Pages with refresh meta tag");
		}
		case ReportDataKeys::RefreshMetaTagImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::RefreshMetaTagCount:
		{
			return 0;
		}
		case ReportDataKeys::RelCanonicalPages:
		{
			return QObject::tr("Pages with rel=\"canonical\"");
		}
		case ReportDataKeys::RelCanonicalPagesImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::RelCanonicalPagesCount:
		{
			return 0;
		}

		// Links
		case ReportDataKeys::BrokenLinks:
		{
			return QObject::tr("Broken links");
		}
		case ReportDataKeys::BrokenLinksImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::BrokenLinksCount:
		{
			const CrawlerEngine::ISequencedStorage* storage = 
				m_sequencedDataCollection->storage(CrawlerEngine::StorageType::Status404StorageType);

			return storage->size();
		}
		case ReportDataKeys::PagesWithLargeAmountOfLinks:
		{
			return QObject::tr("Pages with large amount of links");
		}
		case ReportDataKeys::PagesWithLargeAmountOfLinksImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::PagesWithLargeAmountOfLinksCount:
		{
			return 0;
		}
		case ReportDataKeys::ExternalDofollowLinks:
		{
			return QObject::tr("External dofollow links");
		}
		case ReportDataKeys::ExternalDofollowLinksImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::ExternalDofollowLinksCount:
		{
			return 0;
		}
		case ReportDataKeys::TooLongUrlAddresses:
		{
			return QObject::tr("Too long url addresses");
		}
		case ReportDataKeys::TooLongUrlAddressesImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::TooLongUrlAddressesCount:
		{
			const CrawlerEngine::ISequencedStorage* storage =
				m_sequencedDataCollection->storage(CrawlerEngine::StorageType::VeryLongUrlStorageType);

			return storage->size();
		}

		// Technical factors
		case ReportDataKeys::PagesWithDuplicatedRelCanonical:
		{
			return QObject::tr("Pages with duplicated rel=\"canonical\"");
		}
		case ReportDataKeys::PagesWithDuplicatedRelCanonicalImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::PagesWithDuplicatedRelCanonicalCount:
		{
			return 0;
		}
		case ReportDataKeys::PagesContainsFrames:
		{
			return QObject::tr("Pages that contain frames");
		}
		case ReportDataKeys::PagesContainsFramesImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::PagesContainsFramesCount:
		{
			return 0;
		}
		case ReportDataKeys::PagesWithHtmlErrors:
		{
			return QObject::tr("Pages with errors in HTML");
		}
		case ReportDataKeys::PagesWithHtmlErrorsImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::PagesWithHtmlErrorsCount:
		{
			return 0;
		}
		case ReportDataKeys::PagesWithCssErrorsWarnings:
		{
			return QObject::tr("Pages with warnings and errors in CSS");
		}
		case ReportDataKeys::PagesWithCssErrorsWarningsImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::PagesWithCssErrorsWarningsCount:
		{
			return 0;
		}
		case ReportDataKeys::TooLargePages:
		{
			return QObject::tr("Too large pages");
		}
		case ReportDataKeys::TooLargePagesImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::TooLargePagesCount:
		{
			return 0;
		}

		// Images
		case ReportDataKeys::BrokenImages:
		{
			return QObject::tr("Broken images");
		}
		case ReportDataKeys::BrokenImagesImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::BrokenImagesCount:
		{
			return 0;
		}
		case ReportDataKeys::ImagesWithEmptyAltText:
		{
			return QObject::tr("Images with empty alt text");
		}
		case ReportDataKeys::ImagesWithEmptyAltTextImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::ImagesWithEmptyAltTextCount:
		{
			const CrawlerEngine::ISequencedStorage* storage =
				m_sequencedDataCollection->storage(CrawlerEngine::StorageType::MissingAltTextImageStorageType);

			return storage->size();
		}

		// On Page
		case ReportDataKeys::EmptyTitles:
		{
			return QObject::tr("Pages with empty titles");
		}
		case ReportDataKeys::EmptyTitlesImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::EmptyTitlesCount:
		{
			const CrawlerEngine::ISequencedStorage* storage =
				m_sequencedDataCollection->storage(CrawlerEngine::StorageType::EmptyTitleUrlStorageType);

			return storage->size();
		}
		case ReportDataKeys::DuplicatedTitles:
		{
			return QObject::tr("Pages with duplicated titles");
		}
		case ReportDataKeys::DuplicatedTitlesImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::DuplicatedTitlesCount:
		{
			const CrawlerEngine::ISequencedStorage* storage =
				m_sequencedDataCollection->storage(CrawlerEngine::StorageType::DuplicatedTitleUrlStorageType);

			return storage->size();
		}
		case ReportDataKeys::TooLongTitles:
		{
			return QObject::tr("Too long titles");
		}
		case ReportDataKeys::TooLongTitlesImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::TooLongTitlesCount:
		{
			const CrawlerEngine::ISequencedStorage* storage =
				m_sequencedDataCollection->storage(CrawlerEngine::StorageType::VeryLongTitleUrlStorageType);

			return storage->size();
		}
		case ReportDataKeys::EmptyMetaDescriptions:
		{
			return QObject::tr("Pages with empty meta descriptions");
		}
		case ReportDataKeys::EmptyMetaDescriptionsImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::EmptyMetaDescriptionsCount:
		{
			const CrawlerEngine::ISequencedStorage* storage =
				m_sequencedDataCollection->storage(CrawlerEngine::StorageType::EmptyMetaDescriptionUrlStorageType);

			return storage->size();
		}
		case ReportDataKeys::DuplicatedMetaDescriptions:
		{
			return QObject::tr("Pages with duplicated meta descriptions");
		}
		case ReportDataKeys::DuplicatedMetaDescriptionsImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::DuplicatedMetaDescriptionsCount:
		{
			const CrawlerEngine::ISequencedStorage* storage =
				m_sequencedDataCollection->storage(CrawlerEngine::StorageType::DuplicatedMetaDescriptionUrlStorageType);

			return storage->size();
		}
		case ReportDataKeys::TooLongMetaDescriptions:
		{
			return QObject::tr("Too long meta descriptions");
		}
		case ReportDataKeys::TooLongMetaDescriptionsImage:
		{
			return m_pixmaps[ItemStatus::StatusOK];
		}
		case ReportDataKeys::TooLongMetaDescriptionsCount:
		{
			const CrawlerEngine::ISequencedStorage* storage =
				m_sequencedDataCollection->storage(CrawlerEngine::StorageType::VeryLongMetaDescriptionUrlStorageType);

			return storage->size();
		}
	}

	ASSERT(!"Undefined ReportDataKeys key");
	return QVariant();
}

QList<ReportDataKeys> ReportDataProvider::allKeys() const
{
	return s_placeHolders.keys();
}

QByteArray ReportDataProvider::placeholder(ReportDataKeys reportDataKey) const noexcept
{
	DEBUG_ASSERT(s_placeHolders[reportDataKey] != QByteArray());

	return s_placeHolders[reportDataKey];
}

}