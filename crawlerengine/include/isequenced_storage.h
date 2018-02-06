#pragma once

#include "parsed_page.h"

namespace CrawlerEngine
{

struct RemoveEffects
{
	int removedIndex;
	std::pair<int, int> invalidatedIndicesRange;
};

class ISequencedStorage
{
public:
	friend class SequencedDataCollection;

	virtual ~ISequencedStorage() = default;

	virtual int size() const noexcept = 0;
	virtual void clear() = 0;
	virtual bool empty() const noexcept = 0;
	virtual int find(const ParsedPage*) const noexcept = 0;
	virtual const ParsedPage* operator[](int idx) const noexcept = 0;
	virtual ParsedPage* operator[](int idx) noexcept = 0;
	virtual const ParsedPage* get(int idx) const noexcept = 0;
	virtual ParsedPage* get(int idx) noexcept = 0;

protected:
	virtual void pushBack(const ParsedPagePtr& page) = 0;
	virtual void emplaceBack(ParsedPagePtr&& page) = 0;
	virtual int replace(ParsedPagePtr&& oldPage, ParsedPagePtr&& newPage) = 0;
	virtual RemoveEffects remove(const ParsedPagePtr& page) = 0;
	virtual bool containsPointersWithUseCountGreaterThanOne() const noexcept = 0;
};

}