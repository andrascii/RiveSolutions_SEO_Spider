#pragma once

namespace Common
{

//! Smart pointer similar to std::unique_ptr
//! Calls object's deleteLater instead of operator delete
template<typename T>
class AutoDeleteLaterPtr
{
public:
	explicit AutoDeleteLaterPtr(T* p = nullptr)
		: m_p(p)
	{
	}

	AutoDeleteLaterPtr(AutoDeleteLaterPtr&& other)
		: m_p(other.release())
	{
	}

	~AutoDeleteLaterPtr()
	{
		reset();
	}

	AutoDeleteLaterPtr& operator=(AutoDeleteLaterPtr&& other)
	{
		reset(other.release());
		return *this;
	}

	T* get() const
	{
		return m_p;
	}

	T& operator*() const
	{
		return *get();
	}

	T* operator->() const
	{
		return get();
	}

	operator bool() const
	{
		return m_p != nullptr;
	}

	void reset(T* p = nullptr)
	{
		if (m_p && (m_p != p))
		{
			m_p->deleteLater();
		}
		m_p = p;
	}

	T* release()
	{
		T* p = m_p;
		m_p = nullptr;
		return p;
	}

private:
	T* m_p;
};

template <typename T, typename... Args>
AutoDeleteLaterPtr<T> makeAutoDeleteLater(Args&&... args)
{
	T* t = new T(std::forward<Args>(args)...);
	return AutoDeleteLaterPtr<T>(t);
}

}