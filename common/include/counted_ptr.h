#pragma once

namespace Common
{

// verify that types are complete for increased safety

template<class T> inline void checked_delete(T * x)
{
	// intentionally complex - simplification causes regressions
	typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
	(void) sizeof(type_must_be_complete);
	delete x;
}

template <typename T>
struct cp_inplace_tag
{
};

template <std::size_t N, std::size_t A>
struct cp_aligned_storage
{
	union type
	{
		char data[N];
		//typename boost::type_with_alignment< A >::type align_;
	};
};

template <class T>
class cp_ms_deleter
{
private:

	typedef typename cp_aligned_storage<sizeof(T), std::alignment_of< T >::value >::type storage_type;

	bool m_initialized;
	storage_type m_storage;

private:

	void destroy()
	{
		if (m_initialized)
		{
			reinterpret_cast<T*>(m_storage.data)->~T();
			m_initialized = false;
		}
	}

public:

	cp_ms_deleter() noexcept
		: m_initialized(false)
	{
	}

	template<class A> explicit cp_ms_deleter(A const &) noexcept
		: m_initialized(false)
	{
	}

	// optimization: do not copy storage_
	cp_ms_deleter(cp_ms_deleter const &) noexcept
		: m_initialized(false)
	{
	}

	~cp_ms_deleter() noexcept
	{
		destroy();
	}

	void operator()(T *) noexcept
	{
		destroy();
	}

	static void operator_fn(T*) noexcept // operator() can't be static
	{
	}

	void * address() noexcept
	{
		return m_storage.data;
	}

	void set_initialized() noexcept
	{
		m_initialized = true;
	}
};


class cp_counted_base
{
public:
	cp_counted_base()
		: m_useCount(1)
	{
	}

	virtual void dispose() = 0;

	virtual void destroy()
	{
		delete this;
	}

	void add_ref_copy() noexcept
	{
		++m_useCount;
	}

	void release()
	{
		long newUseCount = --m_useCount;

		if (newUseCount != 0)
		{
			return;
		}

		dispose();
		destroy();
	}

	long use_count() const noexcept
	{
		return m_useCount;
	}

	virtual void* get_untyped_deleter() = 0;

private:
	long m_useCount;
};


template <typename T>
class cp_counted_impl_p final : public cp_counted_base
{
public:
	cp_counted_impl_p(T* pointer)
		: m_pointer(pointer)
	{
	}

	virtual void dispose() override
	{
		delete m_pointer;
	}

	virtual void* get_untyped_deleter() override
	{
		return nullptr;
	}

private:
	T * m_pointer;
};


template <typename T, typename D>
class cp_counted_impl_pd final : public cp_counted_base
{
public:
	cp_counted_impl_pd(T* pointer)
		: m_pointer(pointer)
		, m_deleter(D())
	{
	}

	cp_counted_impl_pd(T* pointer, D deleter)
		: m_pointer(pointer)
		, m_deleter(deleter)
	{
	}

	virtual void dispose() override
	{
		m_deleter(m_pointer);
	}

	virtual void* get_untyped_deleter() override
	{
		return reinterpret_cast<void*>(&m_deleter);
	}

private:
	T * m_pointer;
	D m_deleter;
};


template <typename T, typename D, typename A>
class cp_counted_impl_pda final : public cp_counted_base
{
public:
	cp_counted_impl_pda(T* pointer, D deleter, A allocator)
		: m_pointer(pointer)
		, m_deleter(deleter)
		, m_allocator(allocator)
	{
	}

	cp_counted_impl_pda(T* pointer, A allocator)
		: m_pointer(pointer)
		, m_deleter(D())
		, m_allocator(allocator)
	{
	}

	virtual void dispose() override
	{
		m_deleter(m_pointer);
	}

	virtual void destroy()
	{
		using A2 = typename A::template rebind<cp_counted_impl_pda<Y, D, A>>::other;

		A2 a2(a);

		this->~cp_counted_impl_pda();

		a2.deallocate(this, 1);
	}

	virtual void* get_untyped_deleter() override
	{
		return reinterpret_cast<void*>(&m_deleter);
	}

private:
	T * m_pointer;
	D m_deleter;
	A m_allocator;
};


class cp_count
{
public:
	cp_count()
		: _cp_counted(nullptr)
	{
	}

	template <typename Y>
	explicit cp_count(Y* p)
		: _cp_counted(nullptr)
	{
		try
		{
			_cp_counted = new cp_counted_impl_p<Y>(p);
		}
		catch (...)
		{
			checked_delete(p);
		}
	}

	template <typename Y, typename D>
	cp_count(Y* p, D d)
		: _cp_counted(nullptr)
	{
		try
		{
			_cp_counted = new cp_counted_impl_pd<Y, D>(p, d);
		}
		catch (...)
		{
			d(p);
			throw;
		}
	}

	template <typename Y, typename D>
	cp_count(Y* p, cp_inplace_tag<D>)
		: _cp_counted(nullptr)
	{
		try
		{
			_cp_counted = new cp_counted_impl_pd<Y, D>(p);
		}
		catch (...)
		{
			D::operator_fn(p); // why I should to call this function here?
			throw;
		}
	}

	template <typename Y, typename D, typename A>
	cp_count(Y* p, D d, A a)
		: _cp_counted(nullptr)
	{
		using A2 = typename A::template rebind<cp_counted_impl_pda<Y, D, A>>::other;

		A2 a2(a);

		try
		{
			cp_counted_impl_pda<Y, D, A>* m_cpCounted = a2.allocate(1);
			::new (static_cast<void*>(m_cpCounted)) cp_counted_impl_pda<Y, D, A>(p, d, a);
		}
		catch (...)
		{
			d(p);

			if (_cp_counted != nullptr)
			{
				a2.deallocate(_cp_counted);
			}

			throw;
		}
	}

	template <typename Y, typename D, typename A>
	cp_count(Y* p, cp_inplace_tag<D>, A a)
		: _cp_counted(nullptr)
	{
		using A2 = typename A::template rebind<cp_counted_impl_pda<Y, D, A>>::other;

		A2 a2(a);

		try
		{
			cp_counted_impl_pda<Y, D, A>* m_cpCounted = a2.allocate(1);
			::new (static_cast<void*>(m_cpCounted)) cp_counted_impl_pda<Y, D, A>(p, a);
		}
		catch (...)
		{
			D::operator_fn(p);

			if (_cp_counted != nullptr)
			{
				a2.deallocate(_cp_counted);
			}

			throw;
		}
	}

#if __cplusplus > 201402L
	template <typename Y>
	explicit cp_count(std::auto_ptr<Y>& r)
		: _cp_counted(new sp_counted_impl_p<Y>(r.release()))
	{
	}
#endif

// 	template <typename Y, typename D>
// 	explicit cp_count(std::unique_ptr<Y, D>& r)
// 		: _cp_counted(new sp_counted_impl_pd<typename std::unique_ptr<Y, D>::pointer, D>(r.release(), r.get_deleter()))
// 	{
// 	}

	cp_count(const cp_count& other) noexcept
		: _cp_counted(other._cp_counted)
	{
		if (_cp_counted)
		{
			_cp_counted->add_ref_copy();
		}
	}

	cp_count(cp_count&& other) noexcept
		: _cp_counted(other._cp_counted)
	{
		other._cp_counted = nullptr;
	}

	~cp_count()
	{
		if (_cp_counted)
		{
			_cp_counted->release();
		}
	}

	cp_count& operator=(const cp_count& other)
	{
		if (this == &other)
		{
			return *this;
		}

		if (_cp_counted)
		{
			_cp_counted->release();
		}

		_cp_counted = other._cp_counted;
		_cp_counted->add_ref_copy();

		return *this;
	}

	cp_count& operator=(cp_count&& other)
	{
		if (this == &other)
		{
			return *this;
		}

		if (_cp_counted)
		{
			_cp_counted->release();
		}

		_cp_counted = other._cp_counted;
		other._cp_counted = nullptr;

		return *this;
	}

	void* get_untyped_deleter()
	{
		return _cp_counted->get_untyped_deleter();
	}

	long use_count() const noexcept
	{
		return _cp_counted ? _cp_counted->use_count() : 0;
	}

	void release()
	{
		if (_cp_counted)
		{
			_cp_counted->release();
		}
	}

private:
	cp_counted_base * _cp_counted;
};

template <typename T>
class counted_ptr;

template <typename T>
struct enable_counted_from_this;

template <typename T>
inline void enable_counted(T* pointer, counted_ptr<T>* counted_pointer, typename T::sfinae_helper_type* = nullptr);

inline void enable_counted(const volatile void* const, const volatile void* const)
{
} // do nothing

template <typename T>
class counted_ptr
{
public:
	using element_type = std::remove_extent_t<T>;

	counted_ptr() noexcept
		: _p(nullptr)
	{
	}

	counted_ptr(std::nullptr_t) noexcept
		: counted_ptr()
	{
	}

	template<typename Y>
	explicit counted_ptr(Y* ptr)
		: _p(ptr)
		, _cp_count(ptr)
	{
		enable_counted(_p, this);
	}

	template<typename Y, typename Deleter>
	counted_ptr(Y* p, Deleter d)
		: _p(p)
		, _cp_count(p, d)
	{
		enable_counted(_p, this);
	}

	template<typename Deleter>
	counted_ptr(std::nullptr_t p, Deleter d)
		: _p(p)
		, _cp_count(static_cast<T*>(p), d)
	{
	}

	template<typename Y, typename D, typename A>
	counted_ptr(Y* p, D d, A a)
		: _p(p)
		, _cp_count(p, d, a)
	{
	}

	template<typename D, typename A>
	counted_ptr(std::nullptr_t p, D d, A a)
		: _p(p)
		, _cp_count(p, d, a)
	{
	}

	template<typename Y>
	counted_ptr(const counted_ptr<Y>& r, element_type* p) noexcept
		: _p(p)
		, _cp_count(r._cp_count)
	{
	}

	counted_ptr(const counted_ptr& r) noexcept
		: _p(r._p)
		, _cp_count(r._cp_count)
	{
	}

	template<typename Y>
	counted_ptr(const counted_ptr<Y>& r) noexcept
		: _p(r.m_p)
		, _cp_count(r._cp_count)
	{
	}

	counted_ptr(counted_ptr&& r) noexcept
		: _p(r._p)
		, _cp_count(std::move(r._cp_count))
	{
		r._p = nullptr;
	}

	template <typename Y>
	counted_ptr(counted_ptr<Y>&& r) noexcept
		: _p(r._p)
		, _cp_count(std::move(r._cp_count))
	{
		r._p = nullptr;

		enable_counted(_p, this);
	}

	template <typename Y, typename Deleter>
	counted_ptr(std::unique_ptr<Y, Deleter>&& r)
		: _p(r._p)
		, _cp_count(r)
	{
		enable_counted(_p, this);
	}

	counted_ptr& operator=(const counted_ptr& r) noexcept
	{
		if (this == &r)
		{
			return *this;
		}

		_p = nullptr;
		_cp_count.release();

		_p = r._p;
		_cp_count = r._cp_count;

		return *this;
	}

	template <typename Y>
	counted_ptr& operator=(const counted_ptr<Y>& r) noexcept
	{
		_p = nullptr;
		_cp_count.release();

		_p = r._p;
		_cp_count = r._cp_count;

		return *this;
	}

	counted_ptr& operator=(counted_ptr&& r) noexcept
	{
		if (this == &r)
		{
			return *this;
		}

		_p = r._p;
		_cp_count = std::move(r._cp_count);
		r._p = nullptr;

		return *this;
	}

	template <typename Y>
	counted_ptr& operator=(counted_ptr<Y>&& r) noexcept
	{
		_p = r._p;
		_cp_count = std::move(r._cp_count);
		r._p = nullptr;

		return *this;
	}

	template <typename Y, typename Deleter>
	counted_ptr& operator=(std::unique_ptr<Y, Deleter>&& r)
	{
		if (_cp_count)
		{
			_cp_count.release();
		}

		_p = r.release();
		_cp_count = cp_count(_p, Deleter());

		return *this;
	}

	void reset() noexcept
	{
		_p = nullptr;
		_cp_count.release();
	}

	template <typename Y>
	void reset(Y* p)
	{
		_cp_count.release();
		_cp_count = cp_count(p);
		_p = p;
	}

	template <typename Y, typename D>
	void reset(Y* p, D d)
	{
		_cp_count.release();
		_cp_count = cp_count(p, d);
		_p = p;
	}

	template <typename Y, typename D, typename A>
	void reset(Y* p, D d, A a)
	{
		_cp_count.release();
		_cp_count = cp_count(p, d, a);
		_p = p;
	}

	void swap(counted_ptr& r) noexcept
	{
		std::swap(_p, r._p);
		std::swap(_cp_count, r._cp_count);
	}

	element_type* get() const noexcept
	{
		return _p;
	}

	T& operator*() const noexcept
	{
		return *_p;
	}

	T* operator->() const noexcept
	{
		return _p;
	}

	element_type& operator[](std::ptrdiff_t idx)
	{
		return get()[idx];
	}

	long use_count() const noexcept
	{
		return _cp_count.use_count();
	}

	bool unique() const noexcept
	{
		return _cp_count.use_count() == 1;
	}

	explicit operator bool() const noexcept
	{
		return get() != nullptr;
	}

	friend bool operator==(const counted_ptr& lhs, const counted_ptr& rhs)
	{
		return lhs.get() == rhs.get();
	}

private:
	template <typename T> friend struct enable_counted_from_this;
	template <typename S> friend class counted_ptr;

	template <typename S, typename... Args>
	friend counted_ptr<S> make_counted(Args&&... args);

	template <typename T>
	friend void enable_counted(T* pointer, counted_ptr<T>* counted_pointer, typename T::sfinae_helper_type*);

	counted_ptr(element_type* p, cp_count* count)
		: _p(p)
		, _cp_count(*count)
	{
	}

	void* get_untyped_deleter()
	{
		return _cp_count.get_untyped_deleter();
	}

private:
	element_type * _p;
	cp_count _cp_count;
};


template <typename T>
struct enable_counted_from_this
{
	using sfinae_helper_type = T;
	using element_type = typename counted_ptr<T>::element_type;

	element_type* p;
	cp_count* count;

	counted_ptr<T> make_counted() const
	{
		return counted_ptr<T>(p, count);
	}
};

template <typename T, typename... Args>
counted_ptr<T> make_counted(Args&&... args)
{
	counted_ptr<T> p(nullptr, cp_inplace_tag<cp_ms_deleter<T>>());
	cp_ms_deleter<T>* deleter = static_cast<cp_ms_deleter<T>*>(p.get_untyped_deleter());

	void* inplaceObject = deleter->address();
	::new (inplaceObject) T(std::forward<Args>(args)...);
	deleter->set_initialized();

	T* typedInplaceObject = static_cast<T*>(inplaceObject);
	auto result = counted_ptr<T>(p, typedInplaceObject);

	return result;
}

template <typename T>
inline void enable_counted(T* pointer, counted_ptr<T>* counted_pointer, typename T::sfinae_helper_type*)
{
	if (pointer)
	{
		pointer->p = counted_pointer->_p;
		pointer->count = &counted_pointer->_cp_count;
	}
}

template <typename T, typename U>
counted_ptr<T> cp_static_pointer_cast(const counted_ptr<U>& other)
{
	using element_type = typename counted_ptr<T>::element_type;

	return counted_ptr<T>(other, static_cast<element_type*>(other.get()));
}

template <typename T, typename U>
counted_ptr<T> cp_dynamic_pointer_cast(const counted_ptr<U>& other)
{
	using element_type = typename counted_ptr<T>::element_type;

	T* p = dynamic_cast<element_type*>(other.get());
	return p ? counted_ptr<T>(other, p) : nullptr;
}

template <typename T, typename U>
counted_ptr<T> cp_reinterpret_pointer_cast(const counted_ptr<U>& other)
{
	using element_type = typename counted_ptr<T>::element_type;

	return counted_ptr<T>(other, reinterpret_cast<element_type*>(other.get()));
}

template <typename T, typename U>
counted_ptr<T> cp_const_pointer_cast(const counted_ptr<U>& other)
{
	using element_type = typename counted_ptr<T>::element_type;

	return counted_ptr<T>(other, const_cast<element_type*>(other.get()));
}

}