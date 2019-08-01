#include "stdafx.h"
#include "delegate.h"

namespace CrawlerEngine
{

void Delegate::clear()
{
	m_callbacks.clear();
}

void Delegate::add(const Callback& callback)
{
	m_callbacks.push_back(callback);
}

void Delegate::operator()(const IResponse& response) const
{
	for (const Callback& callback : m_callbacks)
	{
		callback(response);
	}
}

}