#include "lcu_wamp_handler.h"

#include <Poco/Dynamic/Var.h>
#include <Poco/Exception.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>

#include <tuple>
#include <utility>

namespace rito {

using namespace Poco;
using namespace Poco::JSON;
using namespace Poco::Dynamic;

auto parse_call_result(JSON::Array::Ptr message_json_array) -> std::tuple<std::string, std::string>
{
    std::string call_id{message_json_array->get(1).toString()};
    std::string call_result{message_json_array->get(2).toString()};

    return {call_id, call_result};
}

auto parse_call_error(JSON::Array::Ptr message_json_array)
  -> std::tuple<std::string, std::string, std::string>
{
    std::string call_id{message_json_array->get(1).toString()};
    std::string error{message_json_array->get(2).toString()};
    std::string error_description{message_json_array->get(3).toString()};

    return {call_id, error, error_description};
}

auto parse_event(JSON::Array::Ptr message_json_array) -> std::tuple<std::string, std::string>
{
    std::string event_name{message_json_array->get(1).toString()};
    std::string event_data{message_json_array->get(2).toString()};

    return {event_name, event_data};
}

Lcu_wamp_handler::Lcu_wamp_handler()
{
    m_lcu_websocket_handler.register_on_connected_callback([this] {
        on_connected();
    });

    m_lcu_websocket_handler.register_on_disconnected_callback([this] {
        on_disconnected();
    });

    m_lcu_websocket_handler.register_message_callback([this](auto&& message) {
        on_message(std::forward<decltype(message)>(message));
    });
}

void Lcu_wamp_handler::register_on_connected_callback(Connection_callback on_connected) noexcept
{
    m_on_connected_callback = std::move(on_connected);
}

void Lcu_wamp_handler::register_on_disconnected_callback(
  Connection_callback on_disconnected) noexcept
{
    m_on_disconnected_callback = std::move(on_disconnected);
}

void Lcu_wamp_handler::register_call_result_callback(
  Wamp_call_result_callback on_call_result) noexcept
{
    m_on_call_result_callback = std::move(on_call_result);
}

void Lcu_wamp_handler::register_call_error_callback(Wamp_call_error_callback on_call_error) noexcept
{
    m_on_call_error_callback = std::move(on_call_error);
}

void Lcu_wamp_handler::register_event_callback(Wamp_event_callback on_event) noexcept
{
    m_on_event_callback = std::move(on_event);
}

void Lcu_wamp_handler::run() noexcept
{
    m_lcu_websocket_handler.run();
}

void Lcu_wamp_handler::stop() noexcept
{
    m_lcu_websocket_handler.stop();
}

auto Lcu_wamp_handler::subscribe(const std::string& event) noexcept -> bool
{
    std::string message{"["s + std::to_string(static_cast<int>(Wamp_message_type::subscribe)) +
                        ", \"" + event + "\""};
    return m_lcu_websocket_handler.send_message(message);
}

auto Lcu_wamp_handler::unsubscribe(const std::string& event) noexcept -> bool
{
    std::string message{"["s + std::to_string(static_cast<int>(Wamp_message_type::unsubscribe)) +
                        ", \"" + event + "\""};
    return m_lcu_websocket_handler.send_message(message);
}

void Lcu_wamp_handler::on_connected()
{
    if (m_on_connected_callback)
    {
        m_on_connected_callback();
    }
}

void Lcu_wamp_handler::on_disconnected()
{
    if (m_on_disconnected_callback)
    {
        m_on_disconnected_callback();
    }
}

void Lcu_wamp_handler::on_message(const std::string& message)
{
    if (message.empty())
    {
        return;
    }

    dispatch_message(message);
}

void Lcu_wamp_handler::dispatch_message(const std::string& message)
{
    try
    {
        Parser json_parser;

        const auto message_var{json_parser.parse(message)};
        const auto& message_json_array{message_var.extract<JSON::Array::Ptr>()};

        int wamp_message_id{message_json_array->get(0)};
        switch (wamp_message_id)
        {
            case static_cast<int>(Wamp_message_type::call_result):
            {
                const auto& [id, json_data]{parse_call_result(message_json_array)};
                on_call_result(id, json_data);
                break;
            }
            case static_cast<int>(Wamp_message_type::call_error):
            {
                const auto& [id, error, error_description]{parse_call_error(message_json_array)};
                on_call_error(id, error, error_description);
                break;
            }
            case static_cast<int>(Wamp_message_type::event):
            {
                const auto& [event, json_data]{parse_event(message_json_array)};
                on_event(event, json_data);
                break;
            }
        }
    }
    catch (Exception& e)
    {
    }
}

void Lcu_wamp_handler::on_event(const std::string& event, const std::string& json_data)
{
    if (m_on_event_callback)
    {
        m_on_event_callback(event, json_data);
    }
}

void Lcu_wamp_handler::on_call_result(const std::string& identifier, const std::string& json_data)
{
    if (m_on_call_result_callback)
    {
        m_on_call_result_callback(identifier, json_data);
    }
}

void Lcu_wamp_handler::on_call_error(const std::string& identifier,
                                     const std::string& error,
                                     const std::string& error_description)
{
    if (m_on_call_error_callback)
    {
        m_on_call_error_callback(identifier, error, error_description);
    }
}

auto Lcu_wamp_handler::call(const std::string& identifier, const std::string& function) noexcept
  -> bool
{
    std::string message{"["s + std::to_string(static_cast<int>(Wamp_message_type::subscribe)) +
                        ", \"" + identifier + "\", \"" + function + "\"]"};
    return m_lcu_websocket_handler.send_message(message);
}
} // namespace rito
