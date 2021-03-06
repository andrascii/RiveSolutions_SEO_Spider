#pragma once

namespace Common
{

class IStatisticCounter
{
public:
	using CounterValue = std::atomic_ullong;

public:
	virtual ~IStatisticCounter() = default;

	virtual const QString& name() const = 0;
	virtual CounterValue value() const = 0;
	virtual void increment() = 0;
	virtual void commit() = 0;
	virtual void reset() = 0;
};

}