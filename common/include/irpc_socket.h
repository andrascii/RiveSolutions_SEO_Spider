#ifndef irpc_socket_h
#define irpc_socket_h

namespace Common
{

class IRpcSocket
{
public:
    virtual ~IRpcSocket() = default;
    virtual bool connectToServer(const QString& name) = 0;
    virtual void disconnectFromServer() = 0;
    virtual QString serverName() const = 0;
    virtual QString fullServerName() const = 0;
    virtual qint64 readData(char* data, qint64 maxSize) = 0;
    virtual qint64 writeData(const char* data, qint64 maxSize) = 0;
    virtual qint64 peekData(char* data, qint64 maxSize) = 0;
    virtual qint64 transactData(const char* inData, qint64 inSize, char* outData, int outSize) = 0;
    virtual QString errorString() const = 0;
    virtual bool isClosed() = 0;
};

}

#endif
