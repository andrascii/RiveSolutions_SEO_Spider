#pragma once

#include "irequest.h"
#include "iresponse.h"
#include "delegate.h"

namespace CrawlerEngine
{

class Requester;

class Requester : public QObject, public std::enable_shared_from_this<Requester>
{
	Q_OBJECT

public:
	enum State
	{
		StateClear,
		StateWorking,
		StateStopped
	};

	Requester() = default;
	Requester(const Requester& other);
	Requester(const IRequest& request);

	QObject* handler() const noexcept;
	IRequest* request() const noexcept;

	State state() const noexcept;

	void start();
	void stop();
	void processResponse(const IResponse& response) const;

	template <typename ResponseType>
	void addCallback(void(*function)(Requester*, const ResponseType&))
	{
		const auto callback = [this, object, function](const IResponse& response)
		{
			if (response.type() != ResponseType::responseStaticType())
			{
				return;
			}

			function(this, response);
		};

		m_delegate.add(callback);
	}

	template <typename ObjectType, typename ResponseType>
	void addCallback(ObjectType* object, void(ObjectType::*method)(Requester*, const ResponseType&))
	{
		const auto callback = [this, object, method](const IResponse& response)
		{
			if (response.type() != ResponseType::responseStaticType())
			{
				return;
			}

			(object->*method)(this, static_cast<const ResponseType&>(response));
		};

		m_delegate.add(callback);
	}

private:
	std::shared_ptr<IRequest> m_request;

	Delegate m_delegate;

	QObject* m_handler;

	State m_state;
};

using RequesterSharedPtr = std::shared_ptr<Requester>;
using RequesterWeakPtr = std::weak_ptr<Requester>;

}

Q_DECLARE_METATYPE(CrawlerEngine::RequesterSharedPtr);
Q_DECLARE_METATYPE(CrawlerEngine::Requester)