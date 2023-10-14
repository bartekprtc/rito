#pragma once

#include "rito_export.h"

#include "common_types.h"
#include "lcu_websocket_handler.h"

#include <concepts>
#include <format>
#include <iostream>
#include <string>
#include <string_view>

namespace rito {

using namespace std::literals::string_literals;

template <class T>
concept Convertible_to_string = std::convertible_to<T, std::string>;

enum class RITO_EXPORT Wamp_message_type
{
    call = 2,
    call_result = 3,
    call_error = 4,
    subscribe = 5,
    unsubscribe = 6,
    event = 8
};

class RITO_EXPORT Lcu_wamp_handler
{
public:
    Lcu_wamp_handler();

    void register_on_connected_callback(Connection_callback on_connected);
    void register_on_disconnected_callback(Connection_callback on_disconnected);
    void register_call_result_callback(Wamp_call_result_callback on_call_result);
    void register_call_error_callback(Wamp_call_error_callback on_call_error);
    void register_event_callback(Wamp_event_callback on_event);

    void run();

    template <Convertible_to_string... Args>
    bool call(const std::string& id, const std::string& function, Args&&... args);
    bool subscribe(const std::string& event);
    bool unsubscribe(const std::string& event);

private:
    void on_connected();
    void on_disconnected();

    void on_message(const std::string& message);
    void dispatch_message(const std::string& message);

    void on_event(const std::string& event, const std::string& json_data);
    void on_call_result(const std::string& id, const std::string& json_data);
    void on_call_error(const std::string& id,
                       const std::string& error,
                       const std::string& error_description);

private:
    bool m_is_connected;

    Lcu_websocket_handler m_lcu_websocket_handler;

    Connection_callback m_on_connected_callback;
    Connection_callback m_on_disconnected_callback;
    Wamp_call_result_callback m_on_call_result_callback;
    Wamp_call_error_callback m_on_call_error_callback;
    Wamp_event_callback m_on_event_callback;
};

template <Convertible_to_string... Args>
bool Lcu_wamp_handler::call(const std::string& id, const std::string& function, Args&&... args)
{
    std::string argument_list{((", \""s + args + "\""s) + ...)}; // Produces: , "a", "b", "c"

    return m_lcu_websocket_handler.send_message(
      std::format("[{}, \"{}\", \"{}\"{}]",
                  static_cast<int>(Wamp_message_type::call),
                  id,
                  function,
                  argument_list));
}

} // namespace rito
