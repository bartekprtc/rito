#pragma once

#include "internal_constants.h"

#include <Poco/Buffer.h>
#include <Poco/Net/Context.h>
#include <Poco/Net/HTTPBasicCredentials.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/WebSocket.h>

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

namespace rito {

class Https_websocket_client
{
public:
    Https_websocket_client(const Https_websocket_client&) = delete;
    ~Https_websocket_client();

    Https_websocket_client(Https_websocket_client&&) = delete;
    auto operator=(const Https_websocket_client&) -> Https_websocket_client& = delete;
    auto operator=(Https_websocket_client&&) -> Https_websocket_client& = delete;
    Https_websocket_client(std::string host, std::uint16_t port = https_port);

    void set_credentials(const std::string& username, const std::string& password);

    auto is_connected() const -> bool;

    void start();
    void stop();

    auto send_message(std::string_view message) -> bool;
    auto receive_message() -> Poco::Buffer<char>;

private:
    void run();

private:
    bool m_connected;

    std::string m_host;
    std::uint16_t m_port;

    Poco::Net::Context::Ptr m_context;
    Poco::Net::HTTPSClientSession m_session;
    Poco::Net::HTTPBasicCredentials m_credentials;

    Poco::Net::HTTPRequest m_request;
    Poco::Net::HTTPResponse m_response;
    std::unique_ptr<Poco::Net::WebSocket> m_websocket;
};

} // namespace rito
