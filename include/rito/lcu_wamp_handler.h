/**
 * @file lcu_wamp_handler.h
 * @brief Contains definition of Lcu_wamp_handler class.
 */

#pragma once

#include "rito_export.h"

#include "common_types.h"
#include "lcu_websocket_handler.h"

#include <concepts>
#include <iostream>
#include <string>
#include <string_view>

namespace rito {

using namespace std::literals::string_literals;

template <class T>
concept Convertible_to_string = std::convertible_to<T, std::string>;

// @cond DO_NOT_DOCUMENT
enum class Wamp_message_type
{
    call = 2,
    call_result = 3,
    call_error = 4,
    subscribe = 5,
    unsubscribe = 6,
    event = 8
};

// @endcond

/**
 * @brief Handler for LCU connection over websocket with WAMP protocol. Wrapper for
 *   Lcu_websocket_handler class.
 *
 * Handles websocket connection by messages specified by WAMP protocol. Maintains LCU connection
 *   automatically in the background. Offers subscription for five event types:
 *     - Connected to LCU,
 *     - Disconnected from LCU,
 *     - Message received (of WAMP type CALL.RESULT),
 *     - Message received (of WAMP type CALL.ERROR),
 *     - Message received (of WAMP type EVENT).
 *
 *  There are two ways (not mutually exclusive - you may use both at the same time) to use this
 *    class:
 *      - using call() method will result in response being received either via on_call_result
 *        callback or via on_call_error callback,
 *      - using subscribe() and unsubscribe() to manage subscriptions will result in registered
 *        events being received via on_event callback.
 *
 * This class requires at least two threads: one for running the internal loop by calling run() and
 *   the other one for sending messages (via call(), subscribe() and unsubscribe()) and stopping the
 *   loop (via stop()) if necessary.
 */
class RITO_EXPORT Lcu_wamp_handler
{
public:
    Lcu_wamp_handler();

    /**
     * @brief Registers callback function for LCU connected event.
     *
     * @param on_connected - callback function.
     *
     * Registers callback function for LCU connected event. Once callback function is registered and
     *   run() method is called, registered callback will be called every time LCU is found and
     *   connected to. Callback is called from the same thread that called run().
     *
     * Registered callback should be non-blocking and should take as little time to execute as
     *   possible, as it will be called synchronously.
     */
    void register_on_connected_callback(Connection_callback on_connected) noexcept;

    /**
     * @brief Registers callback function for LCU disconnected event.
     *
     * @param on_disconnected - callback function.
     *
     * Registers callback function for LCU disconnected event. Once callback function is registered
     *   and run() method is called, registered callback will be called every time LCU is
     *   disconnected from. Callback is called from the same thread that called run().
     *
     * Registered callback should be non-blocking and should take as little time to execute as
     *   possible, as it will be called synchronously.
     */
    void register_on_disconnected_callback(Connection_callback on_disconnected) noexcept;

    /**
     * @brief Registers callback function for CALL.RESULT message received.
     *
     * @param on_call_result - callback function.
     *
     * Registers callback function for message received. Once callback function is registered,
     *   LCU is connected and run() method is called, registered callback will be called every time
     *   CALL.RESULT message is received over websocket. ID and details of received message will be
     *   passed as parameters to registered callback function. Callback is called from the same
     *   thread that called run().
     *
     * Registered callback should be non-blocking and should take as little time to execute as
     *   possible, as it will be called synchronously.
     */
    void register_call_result_callback(Wamp_call_result_callback on_call_result) noexcept;

    /**
     * @brief Registers callback function for CALL.ERROR message received.
     *
     * @param on_call_error - callback function.
     *
     * Registers callback function for message received. Once callback function is registered,
     *   LCU is connected and run() method is called, registered callback will be called every time
     *   CALL.ERROR message is received over websocket. ID, error type and error description of
     *   received message will be passed as parameters to registered callback function. Callback is
     *   called from the same thread that called run().
     *
     * Registered callback should be non-blocking and should take as little time to execute as
     *   possible, as it will be called synchronously.
     */
    void register_call_error_callback(Wamp_call_error_callback on_call_error) noexcept;

    /**
     * @brief Registers callback function for EVENT message received.
     *
     * @param on_event - callback function.
     *
     * Registers callback function for message received. Once callback function is registered,
     *   LCU is connected and run() method is called, registered callback will be called every time
     *   EVENT message is received over websocket. Event name and its data (taken from received
     *   message) will be passed as parameters to registered callback function. Callback is called
     *   from the same thread that called run().
     *
     * Registered callback should be non-blocking and should take as little time to execute as
     *   possible, as it will be called synchronously.
     */
    void register_event_callback(Wamp_event_callback on_event) noexcept;

    /**
     * @brief Starts internal polling loop. Will not return unless stop() is called from another
     *   thread.
     *
     * This method runs internal loop, which will keep the connection to the LCU, reconnect if that
     *   connection is lost and receive messages. On top of that, if any event handlers are
     *   registered (via any register_* method), respective callbacks will be called from this
     *   thread. If no handlers are registered, all events are discarded.
     *
     * Internal loop can be stopped by calling stop() from another thread.
     */
    void run() noexcept;

    /**
     * @brief Stops internal loop. Drops LCU connection (if such connection was established).
     *
     * This method stops internal loop and drops LCU connection. It should be called from separate
     *   thread. If there is no existing loop running (i.e., none of threads called run()), it does
     *   nothing.
     */
    void stop() noexcept;

    /**
     * @brief Sends message of WAMP type CALL (with arguments) to the LCU (if it's connected).
     *
     * @param id - ID of the CALL request. If response is received, it will have the same ID.
     * @param function - LCU function to be invoked.
     * @param args - parameters of the LCU function. They should be passed in the same order they're
     *   expected by the LCU function.
     *
     * @return true if message is sent successfully, false otherwise.
     *
     * This method sends message to LCU formatted as WAMP CALL message. Expected response is either
     *   CALL.RESULT or CALL.ERROR, depending on whether CALL is successful.
     *
     * This method should be called only if another thread has called run(). If run() was not
     *   called, there is no connection to LCU. Messages are not queued.
     *
     * This method should be called from separate thread.
     */
    template <Convertible_to_string... Args>
    auto call(const std::string& identifier, const std::string& function, Args&&... args) noexcept
      -> bool;

    /**
     * @brief Sends message of WAMP type CALL (without arguments) to the LCU (if it's connected).
     *
     * @param id - ID of the CALL request. If response is received, it will have the same ID.
     * @param function - LCU function to be invoked.
     *
     * @return true if message is sent successfully, false otherwise.
     *
     * This method sends message to LCU formatted as WAMP CALL message. Expected response is either
     *   CALL.RESULT or CALL.ERROR, depending on whether CALL is successful.
     *
     * This method should be called only if another thread has called run(). If run() was not
     *   called, there is no connection to LCU. Messages are not queued.
     *
     * This method should be called from separate thread.
     */
    auto call(const std::string& identifier, const std::string& function) noexcept -> bool;

    /**
     * @brief Sends message of WAMP type SUBSCRIBE to the LCU (if it's connected).
     *
     * @param event - name of the event to be subscribed for.
     *
     * @return true if message is sent successfully, false otherwise.
     *
     * This method sends message to LCU formatted as WAMP SUBSCRIBE message. If successful, it
     *   causes LCU to send events of requested type. They are reported via on_event callback.
     *
     * This method should be called only if another thread has called run(). If run() was not
     *   called, there is no connection to LCU. Messages are not queued.
     *
     * This method should be called from separate thread.
     *
     * @note Subscription information is lost between sessions. If LCU is disconnected from and
     *   then connected to again, you'll need to subscribe to your events again.
     */
    auto subscribe(const std::string& event) noexcept -> bool;

    /**
     * @brief Sends message of WAMP type UNSUBSCRIBE to the LCU (if it's connected).
     *
     * @param event - name of the event to be unsubscribed from.
     *
     * @return true if message is sent successfully, false otherwise.
     *
     * This method sends message to LCU formatted as WAMP UNSUBSCRIBE message. If successful, it
     *   causes LCU to stop sending events of requested type.
     *
     * This method should be called only if another thread has called run(). If run() was not
     *   called, there is no connection to LCU. Messages are not queued.
     *
     * This method should be called from separate thread.
     */
    auto unsubscribe(const std::string& event) noexcept -> bool;

private:
    void on_connected();
    void on_disconnected();

    void on_message(const std::string& message);
    void dispatch_message(const std::string& message);

    void on_event(const std::string& event, const std::string& json_data);
    void on_call_result(const std::string& identifier, const std::string& json_data);
    void on_call_error(const std::string& identifier,
                       const std::string& error,
                       const std::string& error_description);

    bool m_is_connected;

    Lcu_websocket_handler m_lcu_websocket_handler;

    Connection_callback m_on_connected_callback;
    Connection_callback m_on_disconnected_callback;
    Wamp_call_result_callback m_on_call_result_callback;
    Wamp_call_error_callback m_on_call_error_callback;
    Wamp_event_callback m_on_event_callback;
};

template <Convertible_to_string... Args>
auto Lcu_wamp_handler::call(const std::string& identifier,
                            const std::string& function,
                            Args&&... args) noexcept -> bool
{
    std::string argument_list{((", \""s + args + "\""s) + ...)}; // Produces: , "a", "b", "c"

    std::string message{"["s + std::to_string(static_cast<int>(Wamp_message_type::call)) + ", \"" +
                        identifier + "\", \"" + function + "\"" + argument_list + "]"};

    return m_lcu_websocket_handler.send_message(message);
}

} // namespace rito
