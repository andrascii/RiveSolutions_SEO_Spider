#include "message_queue.h"

namespace Common
{

void MessageQueue::addMessage(const Message& message)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_messages.push_back(message);
}

void MessageQueue::addMessage(Message&& message)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_messages.emplace_back(std::move(message));
}

bool MessageQueue::isEmpty() const noexcept
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_messages.empty();
}

Message MessageQueue::extractMessage()
{
	std::lock_guard<std::mutex> locker(m_mutex);
	
	if (!m_messages.size())
	{
		return Message::undefinedMessage();
	}

	Message message = std::move(m_messages.front());
	m_messages.pop_front();

	return message;
}

}