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
		m_requesterPtr.reset(new Requester(request));
		const int dummy[] = { (m_requesterPtr->addCallback(object, callback), 0)... };
	}

private:
	RequesterSharedPtr m_requesterPtr;
};

}