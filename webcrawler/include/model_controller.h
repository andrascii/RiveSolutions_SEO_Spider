#pragma once

#include "page_raw.h"
#include "web_crawler_options.h"

namespace WebCrawler
{

class DataCollection;

class ModelController : public QObject
{
	Q_OBJECT

public:
	ModelController(QObject* parent);
	~ModelController();

	void setWebCrawlerOptions(const WebCrawlerOptions& options);

	void addPageRaw(PageRawPtr pageRaw) noexcept;

	const DataCollection* data() const noexcept;
	DataCollection* data() noexcept;

private:
	void processPageRawUrl(PageRawPtr pageRaw) noexcept;
	void processPageRawTitle(PageRawPtr pageRaw) noexcept;
	void processPageRawMetaDescription(PageRawPtr pageRaw) noexcept;
	void processPageRawMetaKeywords(PageRawPtr pageRaw) noexcept;
	void processPageRawH1(PageRawPtr pageRaw) noexcept;
	void processPageRawH2(PageRawPtr pageRaw) noexcept;
	void processPageRawImage(PageRawPtr pageRaw) noexcept;
	void processPageRawStatusCode(PageRawPtr pageRaw) noexcept;

	void processPageRawHtmlResources(PageRawPtr pageRaw) noexcept;
	void processPageRawResources(PageRawPtr pageRaw) noexcept;
	void fixPageRawResourceType(PageRawPtr pageRaw) noexcept;

private:
	DataCollection* m_data;
	WebCrawlerOptions m_webCrawlerOptions;
};

}