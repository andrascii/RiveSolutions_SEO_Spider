#pragma once

namespace Common
{

template <typename T>
class counted_ptr;

template <typename T>
struct enable_counted_from_this;

template <typename T>
inline void enable_counted(T* pointer, counted_ptr<T>* scopedPointer, typename T::HelperType* = nullptr);

inline void enable_counted(const volatile void* const, const volatile void* const)
{
} // do nothing

template <typename T>
class counted_ptr
{
public:
	using element_type = std::remove_extent_t<T>;

	counted_ptr() noexcept
		: m_counter(nullptr)
		, m_data(nullptr)
		, m_alias(nullptr)
		, m_isCompactAllocation(false)
		, m_deleter(std::default_delete<T>())
	{
	}

	counted_ptr(std::nullptr_t) noexcept
		: counted_ptr()
	{
	}

	template<typename Y>
	explicit counted_ptr(Y* ptr)
		: m_counter(new long(1))
		, m_data(ptr)
		, m_alias(nullptr)
		, m_isCompactAllocation(false)
		, m_deleter(std::default_delete<T>())
	{
		enable_counted(m_data, this);
	}

	template<typename Y, typename Deleter>
	counted_ptr(Y* ptr, Deleter d)
		: m_counter(new long(1))
		, m_data(ptr)
		, m_alias(nullptr)
		, m_isCompactAllocation(false)
		, m_deleter(d)
	{
		enable_counted(m_data, this);
	}

	template<typename Deleter>
	counted_ptr(std::nullptr_t, Deleter d)
		: m_counter(nullptr)
		, m_data(nullptr)
		, m_alias(nullptr)
		, m_isCompactAllocation(false)
		, m_deleter(d)
	{
	}

	template<typename Y>
	counted_ptr(const counted_ptr<Y>& r, element_type* ptr) noexcept
		: m_counter(r.m_counter)
		, m_data(ptr)
		, m_alias(r.m_data)
		, m_isCompactAllocation(r.m_isCompactAllocation)
		, m_deleter(r.m_deleter)
	{
		if (m_counter)
		{
			++*m_counter;
		}
	}

	counted_ptr(const counted_ptr& r) noexcept
		: m_counter(r.m_counter)
		, m_data(r.m_data)
		, m_alias(r.m_alias)
		, m_isCompactAllocation(r.m_isCompactAllocation)
		, m_deleter(r.m_deleter)
	{
		if (m_counter)
		{
			++*m_counter;
		}
	}

	template<typename Y>
	counted_ptr(const counted_ptr<Y>& r) noexcept
		: m_counter(r.m_counter)
		, m_data(r.m_data)
		, m_alias(r.m_alias)
		, m_isCompactAllocation(r.m_isCompactAllocation)
		, m_deleter(r.m_deleter)
	{
		if (m_counter)
		{
			++*m_counter;
		}
	}

	counted_ptr(counted_ptr&& r) noexcept
		: m_counter(r.m_counter)
		, m_data(r.m_data)
		, m_alias(r.m_alias)
		, m_isCompactAllocation(r.m_isCompactAllocation)
		, m_deleter(r.m_deleter)
	{
		r.m_counter = nullptr;
		r.m_data = nullptr;
		r.m_isCompactAllocation = false;
	}

	template<typename Y>
	counted_ptr(counted_ptr<Y>&& r) noexcept
		: m_counter(r.m_counter)
		, m_data(r.m_data)
		, m_alias(r.m_alias)
		, m_isCompactAllocation(r.m_isCompactAllocation)
		, m_deleter(std::move(r.m_deleter))
	{
		r.m_counter = nullptr;
		r.m_data = nullptr;
		r.m_isCompactAllocation = false;
	}

	template<typename Y, typename Deleter>
	counted_ptr(std::unique_ptr<Y, Deleter>&& r)
		: m_counter(new std::size_t(1))
		, m_data(r.release())
		, m_alias(nullptr)
		, m_isCompactAllocation(false)
		, m_deleter(Deleter())
	{
		enable_counted(m_data, this);
	}

	~counted_ptr()
	{
		detach();
	}

	counted_ptr& operator=(const counted_ptr& r) noexcept
	{
		detach();

		m_counter = r.m_counter;
		m_data = r.m_data;
		m_alias = r.m_alias;
		m_isCompactAllocation = r.m_isCompactAllocation;
		m_deleter = r.m_deleter;

		++*m_counter;
	}

	template <typename Y>
	counted_ptr& operator=(const counted_ptr<Y>& r) noexcept
	{
		detach();

		m_counter = r.m_counter;
		m_data = r.m_data;
		m_alias = r.m_alias;
		m_isCompactAllocation = r.m_isCompactAllocation;
		m_deleter = r.m_deleter;

		++*m_counter;
	}

	counted_ptr& operator=(counted_ptr&& r) noexcept
	{
		detach();

		m_counter = r.m_counter;
		m_data = r.m_data;
		m_alias = r.m_alias;
		m_isCompactAllocation = r.m_isCompactAllocation;
		m_deleter = std::move(r.m_deleter);

		r.m_counter = nullptr;
		r.m_data = nullptr;
		r.m_alias = nullptr;
	}

	template <typename Y>
	counted_ptr& operator=(counted_ptr<Y>&& r) noexcept
	{
		detach();

		m_counter = r.m_counter;
		m_data = r.m_data;
		m_alias = r.m_alias;
		m_isCompactAllocation = r.m_isCompactAllocation;
		m_deleter = std::move(r.m_deleter);

		r.m_counter = nullptr;
		r.m_data = nullptr;
		r.m_alias = nullptr;
	}

	template <typename Y, typename Deleter>
	counted_ptr& operator=(std::unique_ptr<Y, Deleter>&& r)
	{
		detach();

		m_counter = new long(1);
		m_data = r.release();
		m_alias = nullptr;
		m_isCompactAllocation = false;
		m_deleter = Deleter();
	}

	void reset() noexcept
	{
		counted_ptr().swap(*this);
	}

	template<typename Y>
	void reset(Y* ptr)
	{
		counted_ptr<T>(ptr).swap(*this);
	}

	template<typename Y, typename Deleter>
	void reset(Y* ptr, Deleter d)
	{
		counted_ptr<T>(ptr, d).swap(*this);
	}

	void swap(counted_ptr& r) noexcept
	{
		std::swap(m_counter, r.m_counter);
		std::swap(m_data, r.m_data);
		std::swap(m_alias, r.m_alias);
		std::swap(m_isCompactAllocation, r.m_isCompactAllocation);
		std::swap(m_deleter, r.m_deleter);
	}

	element_type* get() const noexcept
	{
		return m_alias ? m_alias : m_data;
	}

	T& operator*() const noexcept
	{
		return *get();
	}

	T* operator->() const noexcept
	{
		return get();
	}

	element_type& operator[](std::ptrdiff_t i)
	{
		return get()[i];
	}

	long use_count() const noexcept
	{
		return *m_counter;
	}

	bool unique() const noexcept
	{
		return use_count() == 1;
	}

	explicit operator bool() const noexcept
	{
		return get() != nullptr;
	}

private:
	template <typename T> friend struct enable_counted_from_this;

	template <typename S, typename... Args>
	friend counted_ptr<S> make_counted(Args&&... args);

	template <typename T>
	friend void enable_counted(T* pointer, counted_ptr<T>* countedPointer, typename T::HelperType*);

	counted_ptr(long* counter, T* data, bool isCompactAllocation)
		: m_counter(counter)
		, m_data(data)
		, m_isCompactAllocation(isCompactAllocation)
		, m_deleter(std::default_delete<T>())
	{
		enable_counted(m_data, this);
	}

	void detach()
	{
		if (!m_counter)
		{
			return;
		}

		--*m_counter;

		if (*m_counter > 0)
		{
			return;
		}

		if (m_isCompactAllocation)
		{
			m_data->~T();
			delete[] reinterpret_cast<std::uint8_t*>(m_counter);
		}
		else
		{
			delete m_counter;
			m_deleter(m_data);
		}

		m_counter = nullptr;
		m_data = nullptr;
		m_alias = nullptr;
	}

private:
	long* m_counter;
	T* m_data;
	element_type* m_alias;
	bool m_isCompactAllocation;
	std::function<void(T*)> m_deleter;
};

template <typename T>
struct enable_counted_from_this
{
	using HelperType = T;

	long* m_counter;
	T* m_data;
	bool m_isCompactAllocation;

	counted_ptr<T> counted_from_this() const
	{
		++*m_counter;

		return counted_ptr<T>(m_counter, m_data, m_isCompactAllocation);
	}
};

template <typename T, typename... Args>
counted_ptr<T> make_counted(Args&&... args)
{
	long bufferSize = sizeof(long) + sizeof(T);
	std::uint8_t* buffer = new std::uint8_t[bufferSize];

	long* counter = reinterpret_cast<long*>(buffer);
	T* data = reinterpret_cast<T*>(buffer + sizeof(long));

	*counter = 1;

	new (data) T(std::forward<Args>(args)...);

	return counted_ptr<T>(counter, data, true);
}

template <typename T>
inline void enable_counted(T* pointer, counted_ptr<T>* countedPointer, typename T::HelperType*)
{
	if (pointer)
	{
		pointer->m_data = countedPointer->m_data;
		pointer->m_counter = countedPointer->m_counter;
		pointer->m_isCompactAllocation = countedPointer->m_isCompactAllocation;
	}
}

// The behavior of these functions is undefined unless the corresponding cast from U* to T* is well formed:
// 1) The behavior is undefined unless static_cast<T*>((U*)nullptr) is well formed.
// 2) The behavior is undefined unless dynamic_cast<T*>((U*)nullptr) is well formed.
// 3) The behavior is undefined unless const_cast<T*>((U*)nullptr) is well formed.
// 4) The behavior is undefined unless reinterpret_cast<T*>((U*)nullptr) is well formed.

template <typename T, typename U>
counted_ptr<T> static_pointer_cast(const counted_ptr<U>& r) noexcept
{
	auto p = static_cast<typename counted_ptr<T>::element_type*>(r.get());
	return counted_ptr<T>(r, p);
}

template <typename T, typename U>
counted_ptr<T> dynamic_pointer_cast(const counted_ptr<U>& r) noexcept
{
	if (auto p = dynamic_cast<typename counted_ptr<T>::element_type*>(r.get()); p)
	{
		return counted_ptr<T>(r, p);
	}

	return counted_ptr<T>();
}

template <typename T, typename U>
counted_ptr<T> const_pointer_cast(const counted_ptr<U>& r) noexcept
{
	auto p = const_cast<typename counted_ptr<T>::element_type*>(r.get());
	return counted_ptr<T>(r, p);
}

template <typename T, typename U>
counted_ptr<T> reinterpret_pointer_cast(const counted_ptr<U>& r) noexcept
{
	auto p = reinterpret_cast<typename counted_ptr<T>::element_type*>(r.get());
	return counted_ptr<T>(r, p);
}

}