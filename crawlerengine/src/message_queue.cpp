#include "stdafx.h"
#include "message_queue.h"

namespace CrawlerEngine
{

void MessageQueue::addMessage(const Message& message)
{
	std::lock_guard locker(m_mutex);
	m_messages.push_back(message);
}

void MessageQueue::addMessage(Message&& message)
{
	std::lock_guard locker(m_mutex);
	m_messages.emplace_back(std::move(message));
}

bool MessageQueue::isEmpty() const noexcept
{
	std::lock_guard locker(m_mutex);
	return m_messages.empty();
}

Message MessageQueue::extractMessage()
{
	std::lock_guard locker(m_mutex);

	if (!m_messages.size())
	{
		return Message::undefinedMessage();
	}

	Message message = std::move(m_messages.back());
	m_messages.pop_back();

	return std::move(message);
}

}