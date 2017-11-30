#pragma once

#include "requester.h"

namespace CrawlerEngine
{

class HostInfoProvider : public QObject
{
    Q_OBJECT

public:
    HostInfoProvider();
    ~HostInfoProvider();

    Q_INVOKABLE void handleRequest(RequesterSharedPtr requester);
    Q_INVOKABLE void stopRequestHandling(RequesterSharedPtr requester);

#ifdef Q_OS_WIN

private:
    WSADATA m_wsadata;

#endif
};

}