/**
 * @file Lcu_websocket_handler.h
 * @brief Contains definition of Lcu_websocket_handler class.
 */

#pragma once

#include "rito_export.h"

#include "common_types.h"
#include "lcu_process_handler.h"

#include <memory>
#include <mutex>
#include <string_view>
#include <thread>

namespace rito {

class Https_websocket_client;

/**
 * @brief Handler for websocket communication with LCU (League of Legends client).
 *
 * Handles websocket (duplex) connection to LCU. Automatically maintains connection and reconnects
 *   if necessary. Offers subscription for three event types:
 *     - Connected to LCU
 *     - Disconnected from LCU
 *     - Message (event) received
 *
 * This class requires at least two threads: one for running the internal loop by calling run() and
 *   the other one for sending messages (via send_message()) and stopping the loop (via stop()) if
 *   necessary.
 */
class RITO_EXPORT Lcu_websocket_handler
{
public:
    Lcu_websocket_handler();
    ~Lcu_websocket_handler();

    Lcu_websocket_handler(const Lcu_websocket_handler&) = delete;
    Lcu_websocket_handler(Lcu_websocket_handler&&) = delete;
    auto operator=(const Lcu_websocket_handler&) -> Lcu_websocket_handler& = delete;
    auto operator=(Lcu_websocket_handler&&) -> Lcu_websocket_handler& = delete;

    /**
     * @brief Starts internal polling loop. Will not return unless stop() is called from another
     *   thread.
     *
     * This method runs internal loop, which will keep the connection to the LCU, reconnect if that
     *   connection is lost and receive messages. On top of that, if any event handlers are
     *   registered (via register_message_callback(), register_on_connected_callback() and
     *   register_on_disconnected_callback()), callbacks will be called from this thread. If no
     *   handlers are registered, all events are discarded.
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
     * @brief Registers callback function for message received event.
     *
     * @param on_message_received - callback function.
     *
     * Registers callback function for message received. Once callback function is registered,
     *   LCU is connected and run() method is called, registered callback will be called every time
     *   message is received over websocket. Message contents will be passed as parameter to
     *   registered callback function. Callback is called from the same thread that called run().
     *
     * Registered callback should be non-blocking and should take as little time to execute as
     *   possible, as it will be called synchronously.
     */
    void register_message_callback(Message_callback on_message_received) noexcept;

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
     * @brief Sends message over websocket to LCU.
     *
     * @param message - Message to be sent over websocket.
     * @return true if message is sent successfully, false otherwise.
     *
     * This method should be called only if another thread has called run(). If run() was not
     *   called, there is no connection to LCU. Messages are not queued.
     *
     * This method should be called from separate thread.
     */
    auto send_message(std::string_view message) noexcept -> bool;

private:
    void reconnect();
    auto is_connected() -> bool;

    void notify_connection_state_changed(bool connected);
    void receive_message();
    void reconnect_and_notify();

    void main_loop();

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
