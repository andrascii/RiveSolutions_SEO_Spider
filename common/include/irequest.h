#pragma once

namespace Common
{

enum class RequestType
{
	RequestTypeLoad
};

class IRequest
{
public:
	virtual IRequest* clone() const = 0;
	virtual RequestType requestType() const noexcept = 0;
};

}