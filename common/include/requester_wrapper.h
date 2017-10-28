#pragma once

#include "irequest.h"
#include "requester.h"

namespace Common
{

class RequesterWrapper
{
public:
	Requester* get() const noexcept
	{
		return m_requesterPtr.get();
	}

	Requester* operator->() const noexcept
	{
		return get();
	}

	template <typename ObjectType, typename... ResponseTypes>
	void reset(const IRequest& request, ObjectType* object, void(ObjectType::*...callback)(Requester*, const ResponseTypes))
	{
		static_assert(sizeof...(callback), "Must be at least one callback");

		m_requesterPtr.reset(new Requester(request));
		addCallbackHelper((m_requesterPtr->addCallback(object, callback), 0)...);
	}

private:
	template <typename... Args>
	static void addCallbackHelper(Args...)
	{
	}

private:
	RequesterSharedPtr m_requesterPtr;
};

}