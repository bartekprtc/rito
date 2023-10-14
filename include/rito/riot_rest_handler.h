#pragma once

#include "rito_export.h"

#include "common_types.h"

#include <memory>
#include <string>

namespace rito {

class Https_rest_client;

enum class RITO_EXPORT Region
{
    none,
    br,
    eune,
    euw,
    jp,
    kr,
    lan,
    las,
    na,
    oce,
    tr,
    ru
};

class RITO_EXPORT Riot_rest_handler
{
public:
    Riot_rest_handler(const std::string& apiKey);
    ~Riot_rest_handler();

    inline void set_region(Region region) { m_region = region; }

    Http_response request(Request_type type, const std::string& message);

private:
    Http_response send_request(Request_type type, const std::string& message);
    void attempt_reconnect();

private:
    Region m_region;
    std::string m_api_key;
    std::unique_ptr<Https_rest_client> m_client;
};

} // namespace rito
