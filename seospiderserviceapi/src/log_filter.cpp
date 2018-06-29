#include "log_filter.h"

namespace SeoSpiderServiceApi
{

bool LogFilter::isAvailable(Common::LogLevel level) const
{
	std::lock_guard guard(m_mutex);

	if (!m_functor)
	{
		return true;
	}

	return m_functor(level);
}

}