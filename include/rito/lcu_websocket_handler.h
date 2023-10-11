#pragma once

#include "common_types.h"
#include "lcu_process_handler.h"

#include <memory>
#include <mutex>
#include <string_view>
#include <thread>

namespace rito {

class Https_websocket_client;

class Lcu_websocket_handler
{
public:
    Lcu_websocket_handler();
    ~Lcu_websocket_handler();

    void run();
    void stop();

    void register_message_callback(Message_callback on_message_received);
    void register_on_connected_callback(Connection_callback on_connected);
    void register_on_disconnected_callback(Connection_callback on_disconnected);

    bool send_message(std::string_view message);

private:
    void reconnect();
    bool is_connected();

    void notify_connection_state_changed(bool connected);
    void receive_message();
    void reconnect_and_notify();

    void main_loop();

private:
    std::unique_ptr<Https_websocket_client> m_websocket;
    std::mutex m_socket_mutex;

    bool m_is_state_connected;
    bool m_stop;

    Message_callback m_on_message_received;
    Connection_callback m_on_connected;
    Connection_callback m_on_disconnected;

    Lcu_process_handler m_lcu_process_handler;
};
} // namespace rito
