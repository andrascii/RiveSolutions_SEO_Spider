#include "delegate.h"

namespace Common
{

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