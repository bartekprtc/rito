#include "lcu_websocket_handler.h"

#include "https_websocket_client.h"
#include "lcu_process_handler.h"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace rito {

using namespace std::literals::chrono_literals;

namespace {
constexpr auto event_loop_delay{1s};
}

Lcu_websocket_handler::Lcu_websocket_handler() : m_is_state_connected{false}, m_stop{false} {}

Lcu_websocket_handler::~Lcu_websocket_handler()
{
    stop();
}

void Lcu_websocket_handler::run() noexcept
{
    m_stop = false;

    main_loop();
}

void Lcu_websocket_handler::stop() noexcept
{
    m_stop = true;
    if (is_connected())
    {
        // Get out of receive_message call (any message will do)
        m_websocket->send_message("Exit");
    }
}

void Lcu_websocket_handler::reconnect()
{
    std::lock_guard<std::mutex> socketLock{m_socket_mutex};

    if (m_websocket && !m_websocket->is_connected())
    {
        m_websocket.reset();
    }
    if (!m_websocket)
    {
        Lcu_parameters lcuClientParameters{m_lcu_process_handler.get_lcu_process_parameters()};

        m_websocket =
          std::make_unique<Https_websocket_client>("127.0.0.1", lcuClientParameters.app_port);
        m_websocket->set_credentials("riot", lcuClientParameters.remoting_auth_token);
        m_websocket->start();
    }
}

auto Lcu_websocket_handler::is_connected() -> bool
{
    std::lock_guard<std::mutex> socketLock{m_socket_mutex};
    return m_websocket && m_websocket->is_connected();
}

void Lcu_websocket_handler::notify_connection_state_changed(bool connected)
{
    if (connected && !m_is_state_connected)
    {
        if (m_on_connected)
        {
            m_on_connected();
        }
    }
    else if (!connected && m_is_state_connected)
    {
        if (m_on_disconnected)
        {
            m_on_disconnected();
        }
    }
    m_is_state_connected = connected;
}

void Lcu_websocket_handler::receive_message()
{
    try
    {
        auto res{m_websocket->receive_message()};
        std::string response(res.begin(), res.end());
        if (m_on_message_received)
        {
            m_on_message_received(response);
        }
    }
    catch (...)
    {
    }
}

void Lcu_websocket_handler::reconnect_and_notify()
{
    notify_connection_state_changed(false);
    try
    {
        reconnect();
        notify_connection_state_changed(true);
    }
    catch (...)
    {
        std::this_thread::sleep_for(event_loop_delay);
    }
}

void Lcu_websocket_handler::main_loop()
{
    for (;;)
    {
        if (m_stop)
        {
            {
                std::lock_guard<std::mutex> socket_lock{m_socket_mutex};
                m_websocket.reset();
            }
            notify_connection_state_changed(false);
            break;
        }

        if (is_connected())
        {
            receive_message();
        }
        else
        {
            reconnect_and_notify();
        }
    }
}

void Lcu_websocket_handler::register_message_callback(Message_callback on_message_received) noexcept
{
    m_on_message_received = std::move(on_message_received);
}

void Lcu_websocket_handler::register_on_connected_callback(
  Connection_callback on_connected) noexcept
{
    m_on_connected = std::move(on_connected);
}

void Lcu_websocket_handler::register_on_disconnected_callback(
  Connection_callback on_disconnected) noexcept
{
    m_on_disconnected = std::move(on_disconnected);
}

auto Lcu_websocket_handler::send_message(std::string_view message) noexcept -> bool
{
    if (is_connected())
    {
        std::lock_guard<std::mutex> socket_lock{m_socket_mutex};
        return m_websocket->send_message(message);
    }

    return false;
}

} // namespace rito
