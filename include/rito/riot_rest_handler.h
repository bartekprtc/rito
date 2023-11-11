/**
 * @file riot_rest_handler.h
 * @brief Contains definition of Riot_rest_handler class.
 */

#pragma once

#include "rito_export.h"

#include "common_types.h"

#include <memory>
#include <string>

namespace rito {

class Https_rest_client;

/**
 * @brief Riot region.
 */
enum class Region
{
    none, /*!< Represents no region */
    br,   /*!< Brazil */
    eune, /*!< Europe Nordic & East */
    euw,  /*!< Europe West */
    jp,   /*!< Japan */
    kr,   /*!< Korea */
    lan,  /*!< Latin America North */
    las,  /*!< Latin America South */
    na,   /*!< North America */
    oce,  /*!< Oceania */
    tr,   /*!< Turkey */
    ru    /*!< Russia */
};

/**
 * @brief Handler for REST communication with Riot API.
 */
class RITO_EXPORT Riot_rest_handler
{
public:
    /**
     * @param api_key - Riot API key. Can be either development API key or application-specific
     *                      API key.
     */
    Riot_rest_handler(std::string api_key);
    ~Riot_rest_handler();

    Riot_rest_handler(const Riot_rest_handler&) = delete;
    Riot_rest_handler(Riot_rest_handler&&) = delete;
    auto operator=(const Riot_rest_handler&) -> Riot_rest_handler& = delete;
    auto operator=(Riot_rest_handler&&) -> Riot_rest_handler& = delete;

    /**
     * @brief Sets region. Required to use REST communication.
     *
     * @param region - Region to be set.
     */
    inline void set_region(Region region) { m_region = region; }

    /**
     * @brief Executes REST request synchronously.
     *
     * @param type - type of HTTP request (GET, POST, etc)
     * @param message - request query. Can be either just URL ("/riot/account/v1/accounts/me")
     *                  or concatenated URL + params ("/some/url?param1=value1&param2=value2")
     *
     * @throws No_region_selected_exception
     * @throws Unknown_exception
     * @throws Message_exception
     * @throws Wrong_enum_value_exception
     *
     * @return HTTP response.
     *
     * @pre Region has to be selected using set_region method.
     *
     * Sends HTTP request and returns HTTP response from the server. If the region is not selected,
     *   throws No_region_selected_exception.
     *
     * If provided Request_type has incorrect or unspecified value, Wrong_enum_value_exception is
     *   thrown.
     *
     * If there is some error while sending request to the server or receiving response, either
     *   Message_exception or Unknown_exception is thrown (depending on source of error).
     */
    auto request(Request_type type, const std::string& message) -> Http_response;

private:
    auto send_request(Request_type type, const std::string& message) -> Http_response;
    void attempt_reconnect();

    Region m_region;
    std::string m_api_key;
    std::unique_ptr<Https_rest_client> m_client;
};

} // namespace rito
