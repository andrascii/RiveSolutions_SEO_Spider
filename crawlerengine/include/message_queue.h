#pragma once

#include "message.h"

namespace CrawlerEngine
{
	
class MessageQueue
{
public:
	void addMessage(const Message& message);
	void addMessage(Message&& message);

	bool isEmpty() const noexcept;

	Message extractMessage();

private:
	mutable std::mutex m_mutex;

	std::deque<Message> m_messages;
};

}