#include "riot_rest_handler.h"

#include "exception.h"
#include "https_rest_client.h"

#include <iostream>
#include <memory>

namespace rito {

Riot_rest_handler::Riot_rest_handler(const std::string& apiKey)
  : m_region{Region::none}, m_api_key{apiKey}
{
}

Riot_rest_handler::~Riot_rest_handler() {}

Http_response Riot_rest_handler::request(Request_type type, const std::string& message)
{
    if (m_client)
    {
        try
        {
            return send_request(type, message);
        }
        catch (std::exception& e)
        {
            m_client.reset();
        }
    }

    if (!m_client)
    {
        attempt_reconnect();
        return send_request(type, message);
    }

    throw Unknown_exception{"Unexpected error"};
}

Http_response Riot_rest_handler::send_request(Request_type type, const std::string& message)
{
    Key_value_map credentials;
    credentials.emplace("X-Riot-Token", m_api_key);

    return m_client->request(type, message, credentials);
}

void Riot_rest_handler::attempt_reconnect()
{
    if (m_region == Region::none)
    {
        throw No_region_selected_exception{"You have to select region first"};
    }
    m_client = std::make_unique<Https_rest_client>(get_host_by_region(m_region));
}

std::string get_host_by_region(Region region)
{
    switch (region)
    {
        case Region::none:
            return {};
        case Region::br:
            return "br1.api.riotgames.com";
        case Region::eune:
            return "eun1.api.riotgames.com";
        case Region::euw:
            return "euw1.api.riotgames.com";
        case Region::jp:
            return "jp1.api.riotgames.com";
        case Region::kr:
            return "kr.api.riotgames.com";
        case Region::lan:
            return "la1.api.riotgames.com";
        case Region::las:
            return "la2.api.riotgames.com";
        case Region::na:
            return "na1.api.riotgames.com";
        case Region::oce:
            return "oc1.api.riotgames.com";
        case Region::tr:
            return "tr1.api.riotgames.com";
        case Region::ru:
            return "ru.api.riotgames.com";
    }

    return {};
}

} // namespace rito
