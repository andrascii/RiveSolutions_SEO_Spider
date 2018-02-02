#pragma once

#include "url.h"
#include "status_code.h"
#include "hop.h"

namespace CrawlerEngine
{

class HopsChain
{
public:
	void addHop(const Hop& hop);
	void addHop(Hop&& hop);

	bool hasHopTo(const Url& url) const noexcept;
	std::size_t length() const noexcept;

	Hop& front() noexcept;
	const Hop& front() const noexcept;

	Hop& back() noexcept;
	const Hop& back() const noexcept;

	Hop& operator[](std::size_t idx) noexcept;
	const Hop& operator[](std::size_t idx) const noexcept;

	bool empty() const noexcept;

private:
	std::vector<Hop> m_hopsChain;
};

}