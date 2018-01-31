#pragma once

#include "isequenced_storage.h"
#include "sequenced_storage.h"

namespace CrawlerEngine
{

//
// This solution was introduced in order to avoid race conditions in tests
//

class TestSequencedStorage : public ISequencedStorage
{
public:
	inline virtual int size() const noexcept override
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_sequencedStorage.size();
	}

	inline virtual void clear() override
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		m_sequencedStorage.clear();
	}

	inline virtual bool empty() const noexcept override
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_sequencedStorage.empty();
	}

	inline virtual const ParsedPage* operator[](int idx) const noexcept override
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_sequencedStorage[idx];
	}

	inline virtual ParsedPage* operator[](int idx) noexcept override
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_sequencedStorage[idx];
	}

	virtual const ParsedPage* get(int idx) const noexcept override
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_sequencedStorage[idx];
	}

	virtual ParsedPage* get(int idx) noexcept override
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_sequencedStorage[idx];
	}

protected:
	inline virtual void pushBack(const ParsedPagePtr& page) override
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		m_sequencedStorage.pushBack(page);
	}

	inline virtual void emplaceBack(ParsedPagePtr&& page) override
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		m_sequencedStorage.emplaceBack(std::move(page));
	}

	inline virtual bool containsPointersWithUseCountGreaterThanOne() const noexcept override
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_sequencedStorage.containsPointersWithUseCountGreaterThanOne();
	}

private:
	mutable std::mutex m_mutex;

	SequencedStorage m_sequencedStorage;
};

}