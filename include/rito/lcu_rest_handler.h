/**
 * @file lcu_rest_handler.h
 * @brief Contains definition of Lcu_rest_handler class.
 */

#pragma once

#include "rito_export.h"

#include "common_types.h"
#include "lcu_process_handler.h"

#include <memory>
#include <string>

namespace rito {

class Https_rest_client;

/**
 * @brief Handler for REST communication with LCU (League of Legends client).
 *
 * Handles REST requests and responses to LCU synchronously. Automatically connects to LCU in the
 *   background.
 */
class RITO_EXPORT Lcu_rest_handler
{
public:
    Lcu_rest_handler();
    ~Lcu_rest_handler();

    /**
     * @brief Executes REST request synchronously.
     *
     * @param type - type of HTTP request (GET, POST, etc)
     * @param message - request query. Can be either just URL
     *                  ("/lol-summoner/v1/current-summoner") or concatenated URL + parameters
     *                  ("/some/url?param1=value1&param2=value2")
     *
     * @throws Lcu_not_running_exception (transitive from Lcu_process_handler)
     * @throws Lcu_parameters_exception (transitive from Lcu_process_handler)
     * @throws Path_not_found_exception (Linux only) (transitive from Lcu_process_handler)
     *
     * @throws Unknown_exception
     * @throws Message_exception
     * @throws Wrong_enum_value_exception
     *
     * @return HTTP response.
     *
     * Sends HTTP request and returns HTTP response from the server. Automatically connects to the
     *   LCU in the background using Lcu_process_handler class and its method
     *   Lcu_process_handler::get_lcu_process_parameters(), therefore potentially throws all
     *   exceptions that Lcu_process_handler::get_lcu_process_parameters() throws.
     *
     * If provided Request_type has incorrect or unspecified value, Wrong_enum_value_exception is
     *   thrown.
     *
     * If there is some error while sending request to the server or receiving response, either
     *   Message_exception or Unknown_exception is thrown (depending on source of error).
     */
    Http_response request(Request_type type, const std::string& message);

private:
    void attempt_reconnect();

private:
    std::unique_ptr<Https_rest_client> m_client;
    Lcu_process_handler m_lcu_process_handler;
};

} // namespace rito
