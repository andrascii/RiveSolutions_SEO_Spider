#pragma once

namespace Common
{

class IRpcSocket;

class IRpcServer
{
public:
	virtual ~IRpcServer() = default;

	virtual bool listen(const QString& name) = 0;
	virtual std::shared_ptr<IRpcSocket> nextPendingConnection() = 0;
	virtual QString serverName() const = 0;
	virtual QString fullServerName() const = 0;
	virtual QString errorString() const = 0;
};

}
