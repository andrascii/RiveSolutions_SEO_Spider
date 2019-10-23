#include "stdafx.h"
#include "custom_uri_channel.h"

namespace CrawlerEngine
{

static const QString s_uriChannelSharedMemoryKey("custom_uri_memory");
static const int s_size = 4096;

class SharedMemoryGuard
{
public:
	SharedMemoryGuard(QSharedMemory* memory, bool attach, bool lock)
		: m_memory(memory)
		, m_attached(attach)
		, m_locked(lock)
	{
		if (m_attached && !m_memory->attach(QSharedMemory::ReadWrite))
		{
			m_attached = false;
		}

		if (m_locked && !m_memory->lock())
		{
			m_locked = false;
		}
	}

	~SharedMemoryGuard()
	{
		if (m_locked)
		{
			m_memory->unlock();
		}

		if (m_attached)
		{
			m_memory->detach();
		}
	}

	bool locked() const
	{
		return m_locked;
	}

	bool attached() const
	{
		return m_attached;
	}

private:
	QSharedMemory * m_memory;
	bool m_attached;
	bool m_locked;
};

CustomUrlChannel::CustomUrlChannel(QObject* parent, bool host)
	: QObject(parent)
	, m_sharedMemory(s_uriChannelSharedMemoryKey)
	, m_host(host)
	, m_counter(0)
{
	if (m_host)
	{
		ASSERT(m_sharedMemory.create(s_size, QSharedMemory::ReadWrite));
		// m_sharedMemory.attach(QSharedMemory::ReadOnly);
		ASSERT(m_sharedMemory.lock());
		char* to = static_cast<char*>(m_sharedMemory.data());
		*to = 0;
		ASSERT(m_sharedMemory.unlock());
		startTimer(300);
	}
}

CustomUrlChannel* CustomUrlChannel::instance()
{
	static CustomUrlChannel s_instance(nullptr, true);
	return &s_instance;
}

CustomUrlChannel::~CustomUrlChannel()
{
	m_sharedMemory.detach();
}

bool CustomUrlChannel::writeUri(const QByteArray& uri)
{
	QSharedMemory sharedMemory(s_uriChannelSharedMemoryKey);
	SharedMemoryGuard guard(&sharedMemory, true, true);

	if (!guard.attached())
	{
		// ASSERT(!"Cannot attach to shared memory (probably application is not started)");
		// bad
		return false;
	}

	if (!guard.locked())
	{
		ASSERT(!"Cannot lock shared memory");
		// bad
		return false;
	}

	char* to = static_cast<char*>(sharedMemory.data());

	const int length = static_cast<int>(strlen(uri.constData()));

	if (length > s_size - 2)
	{
		// ASSERT(!"Too long URI");
		// bad
		return false;
	}

	(*to)++;
	*(to + length + 1) = 0;
	std::copy(uri.constData(), uri.constData() + length, to + 1);

	return true;
}
void CustomUrlChannel::timerEvent(QTimerEvent*)
{
	SharedMemoryGuard guard(&m_sharedMemory, false, true);

	if (!guard.locked())
	{
		// bad - retry on the next timer event
		return;
	}

	const char* to = static_cast<char*>(m_sharedMemory.data());
	if (to[0] == m_counter)
	{
		// no new data
		return;
	}

	m_counter = to[0];

	const int length = static_cast<int>(strlen(to + 1));
	const QByteArray result = QByteArray::fromRawData(to + 1, length);
	INFOLOG << "URI Received:" << result;

	emit uriReceived(result);
}
}
