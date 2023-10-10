#pragma once

#include "commonTypes.h"

// Below header (<Poco/Net/Context.h>) transitively includes <winnt.h>
// which defines DELETE macro, which then collides with enum type
#include <Poco/Net/Context.h>
#undef DELETE

#include <Poco/Net/HTTPBasicCredentials.h>
#include <Poco/Net/HTTPSClientSession.h>

#include <cstdint>
#include <string>
#include <unordered_map>

namespace rito {

using KeyValueMap = std::unordered_map<std::string, std::string>;

class HttpsRestClient
{
public:
    HttpsRestClient(const std::string& host, std::uint16_t port = 443);

    void setCredentials(const std::string& username, const std::string& password)
    {
        m_credentials.setUsername(username);
        m_credentials.setPassword(password);
    }

    HttpResponse request(RequestType type,
                         const std::string& pathAndQuery,
                         const KeyValueMap& headers = {});

private:
    void sendRequest(const std::string& type,
                     const std::string& pathAndQuery,
                     const KeyValueMap& headers);
    HttpResponse receiveResponse();

private:
    std::string m_host;
    std::uint16_t m_port;

    const Poco::Net::Context::Ptr m_context;
    Poco::Net::HTTPSClientSession m_session;
    Poco::Net::HTTPBasicCredentials m_credentials;
};

std::string toPocoRequestType(RequestType type);

} // namespace rito
