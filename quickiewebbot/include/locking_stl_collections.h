#pragma once

namespace QuickieWebBot
{

template <typename Key, 
	typename Hash = std::hash<Key>, 
	typename KeyEqual = std::equal_to<Key>,
	typename Allocator = std::allocator<Key>
>
class LockingUnorderedSet
{
public:
	using internal_collection_type = std::unordered_set<Key, Hash, KeyEqual, Allocator>;

	using key_type = typename internal_collection_type::key_type;
	using value_type = typename internal_collection_type::value_type;
	using size_type = typename internal_collection_type::size_type;
	using difference_type = typename internal_collection_type::difference_type;
	using hasher = typename internal_collection_type::hasher;
	using key_equal = typename internal_collection_type::key_equal;
	using allocator_type = typename internal_collection_type::allocator_type;
	using reference = typename internal_collection_type::reference;
	using const_reference = typename internal_collection_type::const_reference;
	using pointer = typename internal_collection_type::pointer;
	using const_pointer = typename internal_collection_type::const_pointer;
	using iterator = typename internal_collection_type::iterator;
	using const_iterator = typename internal_collection_type::const_iterator;

	LockingUnorderedSet()
		: LockingUnorderedSet(size_type(0))
	{
	}

	explicit LockingUnorderedSet(size_type bucket_count, const Hash& hash = Hash(),
		const KeyEqual& equal = KeyEqual(), const Allocator& alloc = Allocator())
		: m_collection(bucket_count, hash, equal, alloc)
	{
	}

	LockingUnorderedSet(size_type bucket_count, const Hash& hash, const Allocator& alloc)
		: LockingUnorderedSet(bucket_count, hash, KeyEqual(), alloc)
	{
	}

	LockingUnorderedSet(size_type bucket_count, const Allocator& alloc)
		: LockingUnorderedSet(bucket_count, Hash(), KeyEqual(), alloc)
	{
	}

	explicit LockingUnorderedSet(const Allocator& alloc)
		: LockingUnorderedSet(size_type(0), alloc)
	{
	}

	template <typename InputIt>
	LockingUnorderedSet(InputIt first, InputIt last, size_type bucket_count = 0, const Hash& hash = Hash(),
		const KeyEqual& equal = KeyEqual(), const Allocator& alloc = Allocator())
		: m_collection(first, last, bucket_count, hash, equal, alloc)
	{
	}

	template <typename InputIt>
	LockingUnorderedSet(InputIt first, InputIt last, size_type bucket_count, const Allocator& alloc)
		: LockingUnorderedSet(first, last, bucket_count, Hash(), KeyEqual(), alloc)
	{
	}

	template <typename InputIt>
	LockingUnorderedSet(InputIt first, InputIt last, size_type bucket_count, const Hash& hash, const Allocator& alloc)
		: LockingUnorderedSet(first, last, bucket_count, hash, KeyEqual(), alloc)
	{
	}

	LockingUnorderedSet(const LockingUnorderedSet& other)
		: m_collection(other.m_collection)
	{
	}

	LockingUnorderedSet(const LockingUnorderedSet& other, const Allocator& alloc)
		: m_collection(other, alloc)
	{
	}

	LockingUnorderedSet(LockingUnorderedSet&& other)
		: m_collection(std::move(other.m_collection))
	{
	}

	LockingUnorderedSet(LockingUnorderedSet&& other, const Allocator& alloc)
		: m_collection(std::move(other.m_collection), alloc)
	{
	}

	LockingUnorderedSet(std::initializer_list<value_type> init, size_type bucket_count = 0,
		const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual(), const Allocator& alloc = Allocator())
		: m_collection(init, bucket_count, hash, equal, alloc)
	{
	}

	LockingUnorderedSet(std::initializer_list<value_type> init, size_type bucket_count, const Allocator& alloc)
		: LockingUnorderedSet(init, bucket_count, Hash(), KeyEqual(), alloc)
	{
	}

	LockingUnorderedSet(std::initializer_list<value_type> init, size_type bucket_count, const Hash& hash, const Allocator& alloc)
		: LockingUnorderedSet(init, bucket_count, hash, KeyEqual(), alloc)
	{
	}

	LockingUnorderedSet& operator=(const LockingUnorderedSet& other)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		m_collection = other.m_collection;
		return *this;
	}

	LockingUnorderedSet& operator=(LockingUnorderedSet&& other) noexcept(
		std::allocator_traits<Allocator>::is_always_equal::value
		&& std::is_nothrow_move_assignable<Hash>::value)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		m_collection = std::move(other.m_collection);
		return *this;
	}

	LockingUnorderedSet& operator=(std::initializer_list<value_type> ilist)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		m_collection = ilist;
		return *this;
	}

	allocator_type get_allocator() const
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.get_allocator();
	}

	iterator begin() noexcept
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.begin();
	}

	const_iterator begin() const noexcept
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.begin();
	}

	const_iterator cbegin() const noexcept
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.cbegin();
	}

	iterator end() noexcept
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.end();
	}

	const_iterator end() const noexcept
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.end();
	}

	const_iterator cend() const noexcept
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.cend();
	}

	bool empty() const noexcept
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.empty();
	}

	size_type size() const noexcept
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.size();
	}

	size_type max_size() const noexcept
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.max_size();
	}

	void clear() noexcept
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.clear();
	}

	std::pair<iterator, bool> insert(const value_type& value)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.insert(value);
	}

	std::pair<iterator, bool> insert(value_type&& value)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.insert(std::move(value));
	}

	iterator insert(const_iterator hint, const value_type& value)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.insert(hint, value);
	}

	iterator insert(const_iterator hint, value_type&& value)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.insert(hint, std::move(value));
	}

	template <typename InputIt>
	void insert(InputIt first, InputIt last)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		m_collection.insert(first, last);
	}

	void insert(std::initializer_list<value_type> ilist)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		m_collection.insert(ilist);
	}

	template <typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.emplace(std::forward<Args&&>(args)...);
	}

	template <typename... Args>
	iterator emplace_hint(const_iterator hint, Args&&... args)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.emplace_hint(hint, std::forward<Args&&>(args)...);
	}

	iterator erase(const_iterator pos)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.erase(pos);
	}

	iterator erase(const_iterator first, const_iterator last)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.erase(first, last);
	}

	size_type erase(const key_type& key)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.erase(key);
	}

	void swap(LockingUnorderedSet& other) noexcept(std::allocator_traits<Allocator>::is_always_equal::value
		&& std::is_nothrow_swappable<Hash>::value
		&& std::is_nothrow_swappable<KeyEqual>::value)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		m_collection.swap(other.m_collection);
	}

	size_type count(const Key& key) const
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.count(key);
	}

	iterator find(const Key& key)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.find(key);
	}

	const_iterator find(const Key& key) const
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.find(key);
	}

	std::pair<iterator, iterator> equal_range(const Key& key)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.equal_range(key);
	}

	std::pair<const_iterator, const_iterator> equal_range(const Key& key) const
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_collection.equal_range(key);
	}

private:
	std::unordered_set<Key, Hash, KeyEqual, Allocator> m_collection;

	mutable std::mutex m_mutex;
};

template <typename ObjectType>
class LocableProxyObject
{
public:
	LocableProxyObject(ObjectType* object, std::mutex& mutex)
		: m_object(object)
		, m_mutex(mutex)
	{
		m_mutex.lock();
	}

	LocableProxyObject(LocableProxyObject&& other) noexcept
		: m_object(other.m_object)
		, m_mutex(std::move(other.m_mutex))
	{
	}

	ObjectType* operator->() noexcept
	{
		return m_object;
	}

	const ObjectType* operator->() const noexcept
	{
		return m_object;
	}

	~LocableProxyObject()
	{
		m_mutex.unlock();
	}

private:
	ObjectType* m_object;
	std::mutex& m_mutex;
};

template <typename ObjectType>
class LockableObject
{
public:
	LocableProxyObject<ObjectType> lock() noexcept
	{
		return LocableProxyObject<ObjectType>(&m_object, m_mutex);
	}

private:
	ObjectType m_object;
	mutable std::mutex m_mutex;
};

}
