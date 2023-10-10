#include "riotRestHandler.h"

#include "exception.h"
#include "httpsRestClient.h"

#include <iostream>
#include <memory>

namespace rito {

RiotRestHandler::RiotRestHandler(const std::string& apiKey)
  : m_region{Region::NONE}, m_apiKey{apiKey}
{
}

RiotRestHandler::~RiotRestHandler() {}

HttpResponse RiotRestHandler::request(RequestType type, const std::string& message)
{
    if (m_client)
    {
        try
        {
            return sendRequest(type, message);
        }
        catch (std::exception& e)
        {
            m_client.reset();
        }
    }

    if (!m_client)
    {
        attemptReconnect();
        return sendRequest(type, message);
    }

    throw UnknownException{"Unexpected error"};
}

HttpResponse RiotRestHandler::sendRequest(RequestType type,
                                                        const std::string& message)
{
    KeyValueMap credentials;
    credentials.emplace("X-Riot-Token", m_apiKey);

    return m_client->request(type, message, credentials);
}

void RiotRestHandler::attemptReconnect()
{
    if (m_region == Region::NONE)
    {
        throw NoRegionSelectedException{"You have to select region first"};
    }
    m_client = std::make_unique<HttpsRestClient>(getHostByRegion(m_region));
}

std::string getHostByRegion(Region region)
{
    switch (region)
    {
        case Region::NONE:
            return {};
        case Region::BR:
            return "br1.api.riotgames.com";
        case Region::EUNE:
            return "eun1.api.riotgames.com";
        case Region::EUW:
            return "euw1.api.riotgames.com";
        case Region::JP:
            return "jp1.api.riotgames.com";
        case Region::KR:
            return "kr.api.riotgames.com";
        case Region::LAN:
            return "la1.api.riotgames.com";
        case Region::LAS:
            return "la2.api.riotgames.com";
        case Region::NA:
            return "na1.api.riotgames.com";
        case Region::OCE:
            return "oc1.api.riotgames.com";
        case Region::TR:
            return "tr1.api.riotgames.com";
        case Region::RU:
            return "ru.api.riotgames.com";
    }

    return {};
}

} // namespace rito
