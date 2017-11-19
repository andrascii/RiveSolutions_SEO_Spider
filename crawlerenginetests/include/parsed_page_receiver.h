#pragma once

namespace CrawlerEngineTests
{

class ParsedPageReceiver : public QObject
{
	Q_OBJECT

public:
	ParsedPageReceiver(const CrawlerEngine::SequencedDataCollection* sequencedDataCollection);
	~ParsedPageReceiver();

	std::future<std::vector<const CrawlerEngine::ParsedPage*>> getParsedPages(int count, int storageType);
	std::vector<const CrawlerEngine::ParsedPage*> storageItems(CrawlerEngine::StorageType storage) const;

	std::future<std::vector<CrawlerEngine::LinksToThisResourceChanges>> getLinksToThisResourceChanges(const CrawlerEngine::ParsedPage* page, int count);

private:
	Q_SLOT void onParsedPageAdded(int row, int type);
	Q_SLOT void onParsedPageLinksToThisResourceChanged(CrawlerEngine::LinksToThisResourceChanges changes);

	void checkWaitCondition(int storageType);
	void checkLinksToThisResourceConditions(const CrawlerEngine::ParsedPage* page);

private:
	QThread * m_receiverThread;
	std::map<int, std::vector<const CrawlerEngine::ParsedPage*>> m_parsedPages;
	std::map<int, std::pair<int, std::promise<std::vector<const CrawlerEngine::ParsedPage*>>>> m_waitConditions;

	std::map<const CrawlerEngine::ParsedPage*, std::vector<CrawlerEngine::LinksToThisResourceChanges>> m_linksToThisResourceChanges;
	std::map<const CrawlerEngine::ParsedPage*, std::pair<int, std::promise<std::vector<CrawlerEngine::LinksToThisResourceChanges>>>> m_linksToThisResourceConditions;

	const CrawlerEngine::SequencedDataCollection* m_sequencedDataCollection;
};

}