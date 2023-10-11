#pragma once

#include "common_types.h"

#include <Poco/Net/Context.h>
#include <Poco/Net/HTTPBasicCredentials.h>
#include <Poco/Net/HTTPSClientSession.h>

#include <cstdint>
#include <string>
#include <unordered_map>

namespace rito {

using Key_value_map = std::unordered_map<std::string, std::string>;

class Https_rest_client
{
public:
    Https_rest_client(const std::string& host, std::uint16_t port = 443);

    void set_credentials(const std::string& username, const std::string& password)
    {
        m_credentials.setUsername(username);
        m_credentials.setPassword(password);
    }

    Http_response request(Request_type type,
                          const std::string& path_and_query,
                          const Key_value_map& headers = {});

private:
    void send_request(const std::string& type,
                      const std::string& path_and_query,
                      const Key_value_map& headers);
    Http_response receive_response();

private:
    std::string m_host;
    std::uint16_t m_port;

    const Poco::Net::Context::Ptr m_context;
    Poco::Net::HTTPSClientSession m_session;
    Poco::Net::HTTPBasicCredentials m_credentials;
};

std::string to_poco_request_type(Request_type type);

} // namespace rito
