#pragma once

namespace CrawlerEngine
{

enum class UserAgentType
{
    Unknown,
    GoogleBot,
    YandexBot,
    MailRuBot,
    YahooBot,
    MsnBot,
    AltaVistaBot,
    RamblerBot,
    AportBot,
    WebAltaBot,
    AnyBot // used for all robots
};

}

Q_DECLARE_METATYPE(CrawlerEngine::UserAgentType);