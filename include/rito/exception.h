/**
 * @file exception.h
 * @brief Contains all exception classes used across Rito library.
 */

#pragma once

#include "rito_export.h"

#include <stdexcept>
#include <string>

namespace rito {

/**
 * @brief General exception class. Every exception should derive from this class.
 */
class RITO_EXPORT Exception : public std::runtime_error
{
public:
    /**
     * @brief Constructor.
     *
     * @param what - exception message
     */
    Exception(const std::string& what) : std::runtime_error{what} {}
};

class RITO_EXPORT Unknown_exception : public Exception
{
public:
    /**
     * @brief Constructor.
     *
     * @param what - exception message
     */
    Unknown_exception(const std::string& what) : Exception{what} {}
};

class RITO_EXPORT Lcu_not_running_exception : public Exception
{
public:
    /**
     * @brief Constructor.
     *
     * @param what - exception message
     */
    Lcu_not_running_exception(const std::string& what) : Exception{what} {}
};

class RITO_EXPORT Lcu_parameters_exception : public Exception
{
public:
    /**
     * @brief Constructor.
     *
     * @param what - exception message
     */
    Lcu_parameters_exception(const std::string& what) : Exception{what} {}
};

class RITO_EXPORT Path_not_found_exception : public Exception
{
public:
    /**
     * @brief Constructor.
     *
     * @param what - exception message
     */
    Path_not_found_exception(const std::string& what) : Exception{what} {}
};

class RITO_EXPORT No_region_selected_exception : public Exception
{
public:
    /**
     * @brief Constructor.
     *
     * @param what - exception message
     */
    No_region_selected_exception(const std::string& what) : Exception{what} {}
};

class RITO_EXPORT Wrong_enum_value_exception : public Exception
{
public:
    /**
     * @brief Constructor.
     *
     * @param what - exception message
     */
    Wrong_enum_value_exception(const std::string& what) : Exception{what} {}
};

class RITO_EXPORT Websocket_exception : public Exception
{
public:
    /**
     * @brief Constructor.
     *
     * @param what - exception message
     */
    Websocket_exception(const std::string& what) : Exception{what} {}
};

class RITO_EXPORT Message_exception : public Exception
{
public:
    /**
     * @brief Constructor.
     *
     * @param what - exception message
     */
    Message_exception(const std::string& what) : Exception{what} {}
};

} // namespace rito
