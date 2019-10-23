#include "stdafx.h"
#include "session.h"

namespace
{

const QString s_defaultSessionName = "unnamed_project";

}

namespace CrawlerEngine
{

Session::Session(State state, const QString& name, QObject* parent)
	: QObject(parent)
	, m_state(state)
	, m_name(name.isEmpty() ? s_defaultSessionName : name)
	, m_hasCustomName(!name.isEmpty())
{
}

Session::State Session::state() const noexcept
{
	return m_state;
}

const QString& Session::name() const noexcept
{
	return m_name;
}

void Session::setName(const QString& name)
{
	m_name = name;

	m_hasCustomName = true;

	emit nameChanged(name);
}

bool Session::hasCustomName() const noexcept
{
	return m_hasCustomName;
}

void Session::setState(State state)
{
	ASSERT(state == StateSaved || state == StateUnsaved || state == StateNone);

	if (state == m_state)
	{
		return;
	}

	m_state = state;

	emit stateChanged(m_state);
}

}