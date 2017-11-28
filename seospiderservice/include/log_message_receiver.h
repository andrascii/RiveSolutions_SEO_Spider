#pragma once

#include "ilogger.h"

namespace SeoSpiderService
{

struct Message
{
    std::uint64_t messageSize;
    SeoSpiderServiceApi::SeverityLevel severityLevel;
    QString message;
};

class LogMessageReceiver : public QObject
{
    Q_OBJECT

public:
    LogMessageReceiver(QObject* parent = nullptr);



signals:
    void messageReceived(Message msg);

protected:
    virtual void timerEvent(QTimerEvent* event) override;

private slots:
    void readMessage();
    void onConnectionClosed();

private:
    QLocalSocket* m_socket;
    
    Message m_message;

    int m_connectionTimerId;
};

}

Q_DECLARE_METATYPE(SeoSpiderService::Message)