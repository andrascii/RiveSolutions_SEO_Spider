#pragma once

namespace Common
{

class IResponse;

class Delegate
{
public:
	typedef std::function<void(const IResponse&)> Callback;

	void clear();
	void add(const Callback& callback);
	void operator()(const IResponse& response) const;

private:
	std::vector<Callback> m_callbacks;
};

}
