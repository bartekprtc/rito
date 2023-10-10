#include "httpsWebsocketClient.h"

#include "exception.h"

#include <Poco/Net/HTTPMessage.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/WebSocket.h>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>

namespace rito {

namespace {
std::uint32_t MAX_FRAME_SIZE{65536};
std::uint8_t MAX_FRAMES_PER_MESSAGE{255};
}

using namespace std::literals::string_literals;

using Poco::Net::Context;
using Poco::Net::HTTPMessage;
using Poco::Net::HTTPRequest;
using Poco::Net::WebSocket;

HttpsWebsocketClient::HttpsWebsocketClient(const std::string& host, std::uint16_t port)
  : m_connected{false},
    m_host{host},
    m_port{port},
    m_context{new Context(Context::TLS_CLIENT_USE, "", "", "", Context::VERIFY_NONE)},
    m_session{m_host, m_port, m_context},
    m_request{HTTPRequest::HTTP_GET, "/", HTTPMessage::HTTP_1_1}
{
}

HttpsWebsocketClient::~HttpsWebsocketClient()
{
    stop();
}

void HttpsWebsocketClient::setCredentials(const std::string& username, const std::string& password)
{
    m_credentials.setUsername(username);
    m_credentials.setPassword(password);
    m_credentials.authenticate(m_request);
}

bool HttpsWebsocketClient::isConnected()
{
    return m_connected;
}

void HttpsWebsocketClient::start()
{
    if (!m_webSocket)
    {
        try
        {
            m_webSocket = std::make_unique<WebSocket>(m_session, m_request, m_response);
            m_webSocket->setMaxPayloadSize(MAX_FRAME_SIZE);
            m_connected = true;
        }
        catch (Poco::Net::WebSocketException& e)
        {
            throw WebSocketException{"Failed to connect to websocket: "s + e.what()};
        }
        catch (std::exception& e)
        {
            throw UnknownException{e.what()};
        }
    }
}

void HttpsWebsocketClient::stop()
{
    if (m_webSocket && m_connected)
    {
        m_connected = false;
        m_webSocket->shutdown();
    }
    m_webSocket.reset();
}

bool HttpsWebsocketClient::sendMessage(std::string_view message)
{
    if (m_webSocket)
    {
        m_webSocket->sendFrame(message.data(), message.size(), WebSocket::FRAME_TEXT);
        return true;
    }

    return false;
}

Poco::Buffer<char> HttpsWebsocketClient::receiveMessage()
{
    if (m_webSocket)
    {
        try
        {
            bool receivedFullMessage{false};
            int flags{};
            Poco::Buffer<char> messageBuffer{0};

            for (uint8_t i{0}; i < MAX_FRAMES_PER_MESSAGE; ++i)
            {
                int frameLength{m_webSocket->receiveFrame(messageBuffer, flags)};

                if (frameLength == 0 && flags == 0)
                {
                    m_connected = false;
                    throw WebSocketException{"Websocket client is disconnected"};
                }

                if (flags & WebSocket::FRAME_FLAG_FIN)
                {
                    receivedFullMessage = true;
                    break;
                }
            }

            if (!receivedFullMessage)
            {
                throw MessageException{"Message is longer than "s +
                                     std::to_string(MAX_FRAME_SIZE * MAX_FRAMES_PER_MESSAGE)};
            }

            return messageBuffer;
        }
        catch (std::exception& e)
        {
            m_connected = false;
            throw UnknownException{e.what()};
        }
    }

    throw WebSocketException{"Websocket client is not started"};
}

} // namespace rito
