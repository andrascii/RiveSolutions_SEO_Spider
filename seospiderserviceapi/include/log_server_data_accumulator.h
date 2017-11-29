#pragma once

#include "ilogger.h"

namespace SeoSpiderServiceApi
{

class LogServerDataAccumulator : public QObject
{
    Q_OBJECT

public:
    LogServerDataAccumulator(QObject* parent);

    void storeLog(const QString& message, SeverityLevel severityLevel);

    Q_SLOT void onNewConnection();
    Q_SLOT void onConnectionClosed();

private:
    void sendMessageToRemoteProcess(const QString& message, SeverityLevel severityLevel);
    void sendAllDataToRemoteProcess();

private:
    enum
    {
        AllLevels = -1
    };

    std::map<int, std::vector<QString>> m_logs;

    QLocalServer* m_server;
    QLocalSocket* m_currentConnectionSocket;
};

}