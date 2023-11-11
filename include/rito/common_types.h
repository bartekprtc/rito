/**
 * @file common_types.h
 * @brief Definitions of public types used across Rito.
 */

#pragma once

#include "rito_export.h"

#include <cstdint>
#include <functional>
#include <string>

/**
 * @namespace rito
 * @brief Everything in Rito library is placed within rito:: namespace.
 */
namespace rito {

/**
 * @brief Type of message callback function (regular message).
 *
 * Function parameters (in order):
 *   - const std::string&: contents of received message (usually JSON)
 */
using Message_callback = std::function<void(const std::string&)>;

/**
 * @brief Type of connection status callback function.
 */
using Connection_callback = std::function<void()>;

/**
 * @brief Type of message callback function (WAMP CALL.RESULT message).
 *
 * Function parameters (in order):
 *   - const std::string&: ID of received CALL.RESULT message
 *   - const std::string&: Details of received CALL.RESULT message (usually JSON)
 */
using Wamp_call_result_callback = std::function<void(const std::string&, const std::string&)>;

/**
 * @brief Type of message callback function (WAMP CALL.ERROR message).
 *
 * Function parameters (in order):
 *   - const std::string&: ID of received CALL.ERROR message
 *   - const std::string&: Error type of received CALL.ERROR message
 *   - const std::string&: Error description of received CALL.ERROR message
 */
using Wamp_call_error_callback =
  std::function<void(const std::string&, const std::string&, const std::string&)>;

/**
 * @brief Type of message callback function (WAMP EVENT message).
 *
 * Function parameters (in order):
 *   - const std::string&: Event of received EVENT message
 *   - const std::string&: Details of received EVENT message
 */
using Wamp_event_callback = std::function<void(const std::string&, const std::string&)>;

/**
 * @brief HTTP request type
 */
enum class Request_type
{
    http_connect, /*!< HTTP CONNECT method */
    http_delete,  /*!< HTTP DELETE method */
    http_get,     /*!< HTTP GET method */
    http_head,    /*!< HTTP HEAD method */
    http_options, /*!< HTTP OPTIONS method */
    http_patch,   /*!< HTTP PATCH method */
    http_post,    /*!< HTTP POST method */
    http_put,     /*!< HTTP PUT method */
    http_trace    /*!< HTTP TRACE method */
};

/**
 * @brief HTTP response.
 */
struct RITO_EXPORT Http_response
{
    /**
     * @brief Holds status code (200 - OK, 404 - not found, etc.)
     */
    std::int32_t status{};

    /**
     * @brief Holds reason (if status is different than 200)
     */
    std::string reason{};

    /**
     * @brief Holds payload of HTTP response.
     */
    std::string response{};
};

} // namespace rito
