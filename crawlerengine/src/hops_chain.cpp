#include "stdafx.h"
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


HopsChain::iterator HopsChain::begin() noexcept
{
	return m_hopsChain.begin();
}

HopsChain::const_iterator HopsChain::begin() const noexcept
{
	return m_hopsChain.begin();
}

HopsChain::const_iterator HopsChain::cbegin() const noexcept
{
	return m_hopsChain.cbegin();
}

HopsChain::iterator HopsChain::end() noexcept
{
	return m_hopsChain.end();
}

HopsChain::const_iterator HopsChain::end() const noexcept
{
	return m_hopsChain.end();
}

HopsChain::const_iterator HopsChain::cend() const noexcept
{
	return m_hopsChain.cend();
}

}