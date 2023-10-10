#pragma once

#include <Poco/Buffer.h>
#include <Poco/Net/Context.h>
#include <Poco/Net/HTTPBasicCredentials.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/WebSocket.h>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

namespace rito {

class HttpsWebsocketClient
{
public:
    HttpsWebsocketClient(const std::string& host, std::uint16_t port = 443);
    ~HttpsWebsocketClient();

    void setCredentials(const std::string& username, const std::string& password);

    bool isConnected();

    void start();
    void stop();

    bool sendMessage(std::string_view message);
    Poco::Buffer<char> receiveMessage();

private:
    void run();

private:
    bool m_connected;

    std::string m_host;
    std::uint16_t m_port;

    const Poco::Net::Context::Ptr m_context;
    Poco::Net::HTTPSClientSession m_session;
    Poco::Net::HTTPBasicCredentials m_credentials;

    Poco::Net::HTTPRequest m_request;
    Poco::Net::HTTPResponse m_response;
    std::unique_ptr<Poco::Net::WebSocket> m_webSocket;
};

} // namespace rito
