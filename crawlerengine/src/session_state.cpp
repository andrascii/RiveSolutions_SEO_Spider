#include "session_state.h"

namespace CrawlerEngine
{

SessionState::SessionState(State state, QObject* parent)
	: QObject(parent)
	, m_state(state)
{
}

void SessionState::setState(int state)
{
	ASSERT(state == StateSaved || state == StateUnsaved);

	if (state == m_state)
	{
		return;
	}

	m_state = static_cast<State>(state);

	emit stateChanged(m_state);
}

}