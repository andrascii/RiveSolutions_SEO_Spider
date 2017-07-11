#pragma once

namespace Common
{

template <typename T>
class PimplHelper
{
public:
	PimplHelper()
		: m_ptr(nullptr)
	{
	}

	PimplHelper(T* ptr)
		: m_ptr(ptr)
	{
	}

	~PimplHelper()
	{
		delete m_ptr;
	}

	template <typename... Args>
	PimplHelper(Args&&... args)
		: m_ptr(new T{ std::forward<Args>(args)... })
	{
	}

	PimplHelper(const PimplHelper&) = delete;
	PimplHelper(PimplHelper&&) = delete;

	void reset(T* ptr)
	{
		m_ptr = ptr;
	}

	const T* get() const noexcept
	{
		return m_ptr;
	}

	T* get() noexcept
	{
		return m_ptr;
	}

	const T* operator->() const noexcept
	{
		return m_ptr;
	}

	T* operator->() noexcept
	{
		return m_ptr;
	}

	T& operator*() noexcept
	{
		return m_ptr;
	}

	const T& operator*() const noexcept
	{
		return m_ptr;
	}

private:
	T* m_ptr;
};

}
