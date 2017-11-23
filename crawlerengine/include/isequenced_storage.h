#pragma once

#include "parsed_page.h"

namespace CrawlerEngine
{

class ISequencedStorage
{
public:
	friend class SequencedDataCollection;

	virtual int size() const noexcept = 0;
	virtual void clear() = 0;
	virtual bool empty() const noexcept = 0;
	virtual const ParsedPage* operator[](int idx) const noexcept = 0;
	virtual ParsedPage* operator[](int idx) noexcept = 0;

protected:
	virtual void pushBack(const ParsedPagePtr& page) = 0;
	virtual void emplaceBack(ParsedPagePtr&& page) = 0;
	virtual bool containsPointersWithUseCountGreaterThanOne() const noexcept = 0;
};

}