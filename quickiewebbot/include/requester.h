#pragma once

#include "request_types.h"

namespace QuickieWebBot
{

class Requester
{
public:
	Requester(IRequest::RequestType requestType, QObject* targetObject, QMetaMethod& metaMethod)
		: m_targetObject(targetObject)
		, m_metaMethod(metaMethod)
	{

#ifdef DEBUG
		checkMethodExistence(targetObject, metaMethod);
#endif

	}

	void send()
	{

	}

#ifdef DEBUG
	void checkMethodExistence(QObject* targetObject, QMetaMethod& metaMethod) const noexcept
	{
		bool result = false;

		const int invokableMethodCount = 
			targetObject->metaObject()->methodCount();

		for (int i = 0; i < invokableMethodCount; ++i)
		{
			if (targetObject->metaObject()->method(i) == metaMethod)
			{
				result = true;
				break;
			}
		}

		assert(result);
	}
#endif

private:
	QObject* m_targetObject;
	QMetaMethod m_metaMethod;
};

}
