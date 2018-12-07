#pragma once

#include "url.h"
#include "hop.h"

namespace CrawlerEngine
{

class HopsChain
{
public:
	using iterator = std::vector<Hop>::iterator;
	using const_iterator = std::vector<Hop>::const_iterator;

	void addHop(const Hop& hop);
	void addHop(Hop&& hop);

	bool hasHopTo(const Url& url) const noexcept;
	std::size_t length() const noexcept;

	Hop& firstHop() noexcept;
	const Hop& firstHop() const noexcept;

	Hop& lastHop() noexcept;
	const Hop& lastHop() const noexcept;

	Hop& operator[](std::size_t idx) noexcept;
	const Hop& operator[](std::size_t idx) const noexcept;

	bool empty() const noexcept;


	iterator begin() noexcept;
	const_iterator begin() const noexcept;
	const_iterator cbegin() const noexcept;

	iterator end() noexcept;
	const_iterator end() const noexcept;
	const_iterator cend() const noexcept;

private:
	std::vector<Hop> m_hopsChain;
};

Q_DECLARE_METATYPE(HopsChain)

}