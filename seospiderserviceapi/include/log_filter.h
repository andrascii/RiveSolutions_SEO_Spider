#pragma once

#include "pipe_message.h"

namespace SeoSpiderServiceApi
{

class LogFilter final
{
public:
	bool isAvailable(Common::SeverityLevel level) const;

	template <typename F>
	void setFilter(F&& f)
	{
		std::lock_guard guard(m_mutex);
		m_functor = std::forward<F>(f);
	}

private:
	mutable std::mutex m_mutex;
	std::function<bool(Common::SeverityLevel)> m_functor;
};

}