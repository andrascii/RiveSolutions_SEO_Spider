#include "hops_chain.h"

namespace CrawlerEngine
{

void HopsChain::addHop(const Hop& hop)
{
	m_hopsChain.push_back(hop);
}

void HopsChain::addHop(Hop&& hop)
{
	m_hopsChain.emplace_back(std::move(hop));
}

bool HopsChain::hasHopTo(const Url& url) const noexcept
{
	const auto predicate = [&url](const Hop& hop)
	{ 
		return url.compare(hop.url());
	};

	return std::find_if(m_hopsChain.begin(), m_hopsChain.end(), predicate) != m_hopsChain.end();
}

std::size_t HopsChain::length() const noexcept
{
	return m_hopsChain.size();
}

Hop& HopsChain::firstHop() noexcept
{
	return m_hopsChain.front();
}

const Hop& HopsChain::firstHop() const noexcept
{
	return m_hopsChain.front();
}

Hop& HopsChain::lastHop() noexcept
{
	return m_hopsChain.back();
}

const Hop& HopsChain::lastHop() const noexcept
{
	return m_hopsChain.back();
}

Hop& HopsChain::operator[](std::size_t idx) noexcept
{
	return m_hopsChain[idx];
}

const Hop& HopsChain::operator[](std::size_t idx) const noexcept
{
	return m_hopsChain[idx];
}

bool HopsChain::empty() const noexcept
{
	return m_hopsChain.empty();
}

}