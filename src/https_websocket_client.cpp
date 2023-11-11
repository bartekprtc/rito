#include "https_websocket_client.h"

#include "exception.h"

#include <Poco/Exception.h>
#include <Poco/Net/HTTPMessage.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/WebSocket.h>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>

namespace rito {

namespace {
constexpr std::uint32_t max_frame_size{65536};
constexpr std::uint8_t max_frames_per_message{255};
}

using namespace std::literals::string_literals;

using Poco::Net::Context;
using Poco::Net::HTTPMessage;
using Poco::Net::HTTPRequest;
using Poco::Net::WebSocket;

Https_websocket_client::Https_websocket_client(std::string host, std::uint16_t port)
  : m_connected{false},
    m_host{std::move(host)},
    m_port{port},
    m_context{new Context(Context::TLS_CLIENT_USE, "", "", "", Context::VERIFY_NONE)},
    m_session{m_host, m_port, m_context},
    m_request{HTTPRequest::HTTP_GET, "/", HTTPMessage::HTTP_1_1}
{
}

Https_websocket_client::~Https_websocket_client()
{
    stop();
}

void Https_websocket_client::set_credentials(const std::string& username,
                                             const std::string& password)
{
    m_credentials.setUsername(username);
    m_credentials.setPassword(password);
    m_credentials.authenticate(m_request);
}

auto Https_websocket_client::is_connected() const -> bool
{
    return m_connected;
}

void Https_websocket_client::start()
{
    if (!m_websocket)
    {
        try
        {
            m_websocket = std::make_unique<WebSocket>(m_session, m_request, m_response);
            m_websocket->setMaxPayloadSize(max_frame_size);
            m_connected = true;
        }
        catch (Poco::Net::WebSocketException& e)
        {
            throw Websocket_exception{"Failed to connect to websocket: "s + e.what()};
        }
        catch (std::exception& e)
        {
            throw Unknown_exception{e.what()};
        }
    }
}

void Https_websocket_client::stop()
{
    if (m_websocket && m_connected)
    {
        m_connected = false;
        m_websocket->shutdown();
    }
    m_websocket.reset();
}

auto Https_websocket_client::send_message(std::string_view message) -> bool
{
    if (m_websocket)
    {
        m_websocket->sendFrame(message.data(), static_cast<int>(message.size()), WebSocket::FRAME_TEXT);
        return true;
    }

    return false;
}

auto Https_websocket_client::receive_message() -> Poco::Buffer<char>
{
    if (m_websocket)
    {
        try
        {
            bool received_full_message{false};
            int flags{};
            Poco::Buffer<char> message_buffer{0};

            for (uint8_t i{0}; i < max_frames_per_message; ++i)
            {
                int frame_length{m_websocket->receiveFrame(message_buffer, flags)};

                if (frame_length == 0 && flags == 0)
                {
                    m_connected = false;
                    throw Websocket_exception{"Websocket client is disconnected"};
                }

                if (flags & WebSocket::FRAME_FLAG_FIN)
                {
                    received_full_message = true;
                    break;
                }
            }

            if (!received_full_message)
            {
                throw Message_exception{"Message is longer than "s +
                                        std::to_string(max_frame_size * max_frames_per_message)};
            }

            return message_buffer;
        }
        catch (Poco::TimeoutException& e)
        {
        }
        catch (Websocket_exception& e)
        {
            throw;
        }
        catch (Message_exception& e)
        {
            throw;
        }
        catch (std::exception& e)
        {
            m_connected = false;
            throw Unknown_exception{e.what()};
        }
    }

    throw Websocket_exception{"Websocket client is not started"};
}

} // namespace rito
