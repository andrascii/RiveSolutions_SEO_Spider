#pragma once

#include "isequenced_storage.h"
#include "parsed_page.h"

#undef max

namespace CrawlerEngine
{

class SequencedStorage : public ISequencedStorage
{
public:
	friend class TestSequencedStorage;

	virtual int size() const noexcept override
	{
		const auto size = m_pages.size();

		ASSERT(size <= std::numeric_limits<int>::max() && "Integer overflow");

		return static_cast<int>(size);
	}

	virtual void clear() override
	{
		m_pages.clear();
	}

	virtual bool empty() const noexcept override
	{
		return m_pages.empty();
	}

	virtual int find(const ParsedPage* page) const noexcept override
	{
		auto pageIterator = std::find_if(m_pages.begin(), m_pages.end(), 
			[&page](ParsedPagePtr pagePtr)
		{
			return pagePtr.get() == page;
		});

		if(pageIterator == m_pages.end())
		{
			return -1;
		}

		return std::distance(m_pages.begin(), pageIterator);
	}

	virtual const ParsedPage* operator[](int idx) const noexcept override
	{
		ASSERT(idx >= 0 && idx < m_pages.size());

		return m_pages[idx].get();
	}

	virtual ParsedPage* operator[](int idx) noexcept override
	{
		const SequencedStorage& thisConst = *static_cast<SequencedStorage const * const>(this);

		return const_cast<ParsedPage*>(thisConst[idx]);
	}

	virtual const ParsedPage* get(int idx) const noexcept override
	{
		ASSERT(idx >= 0 && idx < m_pages.size());

		return m_pages[idx].get();
	}

	virtual ParsedPage* get(int idx) noexcept override
	{
		const SequencedStorage& thisConst = *static_cast<SequencedStorage const * const>(this);

		return const_cast<ParsedPage*>(thisConst[idx]);
	}

protected:
	virtual void pushBack(const ParsedPagePtr& page) override
	{
		DEBUG_ASSERT(m_pages.size() < std::numeric_limits<int>::max());

		m_pages.push_back(page);
	}

	virtual void emplaceBack(ParsedPagePtr&& page) override
	{
		DEBUG_ASSERT(m_pages.size() < std::numeric_limits<int>::max());

		m_pages.emplace_back(std::move(page));
	}

	virtual RemoveEffects remove(const ParsedPagePtr& page) override
	{
		const auto pageIterator =  std::find(m_pages.begin(), m_pages.end(), page);

		RemoveEffects removeEffects;

		if (pageIterator == m_pages.end())
		{
			removeEffects.removedIndex = -1;
			removeEffects.invalidatedIndicesRange.first = -1;
			removeEffects.invalidatedIndicesRange.second = -1;

			return removeEffects;
		}

		removeEffects.removedIndex = std::distance(m_pages.begin(), pageIterator);
		const auto nextElement = m_pages.erase(pageIterator);

		if (!removeEffects.removedIndex)
		{
			removeEffects.invalidatedIndicesRange.first = 0;
			removeEffects.invalidatedIndicesRange.second = 0;
			
			return removeEffects;
		}

		removeEffects.invalidatedIndicesRange.first = std::distance(m_pages.begin(), nextElement);

		const std::size_t upperBound = m_pages.size() - 1;
		ASSERT(upperBound <= std::numeric_limits<int>::max() && "Integer overflow");

		removeEffects.invalidatedIndicesRange.second = static_cast<int>(upperBound);

		return removeEffects;
	}

	virtual int replace(ParsedPagePtr&& oldPage, ParsedPagePtr&& newPage) override
	{
		const auto pageIterator = std::find(m_pages.begin(), m_pages.end(), oldPage);

		if (pageIterator == m_pages.end())
		{
			return -1;
		}

		const int replacedIndex = std::distance(m_pages.begin(), pageIterator);
		*pageIterator = std::move(newPage);

		return replacedIndex;
	}

	virtual bool containsPointersWithUseCountGreaterThanOne() const noexcept override;

private:
	std::vector<ParsedPagePtr> m_pages;
};

}