#pragma once

#include "irequest.h"
#include "requester.h"
#include "meta_helpers.h"

namespace CrawlerEngine
{

class RequesterWrapper
{
public:
	~RequesterWrapper()
	{
		reset();
	}

	Requester* get() const noexcept
	{
		return m_requesterPtr.get();
	}

	Requester* operator->() const noexcept
	{
		return get();
	}

	operator bool() const noexcept
	{
		return static_cast<bool>(m_requesterPtr);
	}

	void reset()
	{
		m_requesterPtr.reset();
	}

	void reset(const IRequest& request)
	{
		m_requesterPtr.reset(new Requester(request));
	}

	template <typename ObjectType, typename... ResponseTypes>
	void reset(const IRequest& request, ObjectType* object, void(ObjectType::*...callback)(Requester*, const ResponseTypes))
	{
		static_assert(sizeof...(callback), "Must be at least one callback");

		m_requesterPtr.reset(new Requester(request));

		const auto binder = [this, object](auto&& callback)
		{
			m_requesterPtr->addCallback(object, callback);
		};

		MetaHelpers::callForAllArgumentsInPack(binder, std::forward<decltype(callback)>(callback)...);
	}

private:
	RequesterSharedPtr m_requesterPtr;
};

}