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

	inline virtual int size() const noexcept override
	{
		return static_cast<int>(m_pages.size());
	}

	inline virtual void clear() override
	{
		m_pages.clear();
	}

	inline virtual bool empty() const noexcept override
	{
		return m_pages.empty();
	}

	inline virtual const ParsedPage* operator[](int idx) const noexcept override
	{
		ASSERT(idx >= 0 && idx < m_pages.size());

		return m_pages[idx].get();
	}

	inline virtual ParsedPage* operator[](int idx) noexcept override
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
	inline virtual void pushBack(const ParsedPagePtr& page) override
	{
		DEBUG_ASSERT(m_pages.size() < std::numeric_limits<int>::max());

		m_pages.push_back(page);
	}

	inline virtual void emplaceBack(ParsedPagePtr&& page) override
	{
		DEBUG_ASSERT(m_pages.size() < std::numeric_limits<int>::max());

		m_pages.emplace_back(std::move(page));
	}

	virtual bool containsPointersWithUseCountGreaterThanOne() const noexcept override;

private:
	std::vector<ParsedPagePtr> m_pages;
};

}