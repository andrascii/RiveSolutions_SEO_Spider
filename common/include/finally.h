#pragma once

namespace Common
{

class Finally final
{
public:
	Finally(const std::function<void()>& function)
		: m_function(function)
	{
	}

	~Finally()
	{
		m_function();
	}

private:
	std::function<void()> m_function;
};

}