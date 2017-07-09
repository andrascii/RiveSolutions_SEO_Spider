#pragma once

namespace QuickieWebBot
{

struct IRequest
{
	enum RequestType
	{
		FirstRequestType,
		SecondRequestType
	};

	virtual int type() const noexcept = 0;
};



}
