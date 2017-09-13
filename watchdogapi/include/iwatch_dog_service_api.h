#pragma once

namespace WatchDogApi
{

class IWatchDogServiceApi
{
public:
	virtual void init() noexcept = 0;
	virtual void free() const noexcept = 0;
	virtual void setProcessExceptionHandlers() const noexcept = 0;
	virtual void setThreadExceptionHandlers() const noexcept = 0;

	//
	// TODO: move asserts to this interface in order to
	// We can handle program crashes by asserts
	// 
};

}