#pragma once

#include "commonTypes.h"

#include <memory>
#include <string>

namespace rito {

class HttpsRestClient;

enum class Region
{
    NONE,
    BR,
    EUNE,
    EUW,
    JP,
    KR,
    LAN,
    LAS,
    NA,
    OCE,
    TR,
    RU
};

class RiotRestHandler
{
public:
    RiotRestHandler(const std::string& apiKey);
    ~RiotRestHandler();

    inline void setRegion(Region region) { m_region = region; }

    HttpResponse request(RequestType type, const std::string& message);

private:
    HttpResponse sendRequest(RequestType type, const std::string& message);
    void attemptReconnect();

private:
    Region m_region;
    std::string m_apiKey;
    std::unique_ptr<HttpsRestClient> m_client;
};

std::string getHostByRegion(Region region);

} // namespace rito
