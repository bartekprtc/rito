#pragma once

#include <stdexcept>
#include <string>

namespace rito {

class Exception : public std::runtime_error
{
public:
    Exception(const std::string& what) : std::runtime_error{what} {}
};

class Unknown_exception : public Exception
{
public:
    Unknown_exception(const std::string& what) : Exception{what} {}
};

class Lcu_not_running_exception : public Exception
{
public:
    Lcu_not_running_exception(const std::string& what) : Exception{what} {}
};

class Lcu_parameters_exception : public Exception
{
public:
    Lcu_parameters_exception(const std::string& what) : Exception{what} {}
};

class Path_not_found_exception : public Exception
{
public:
    Path_not_found_exception(const std::string& what) : Exception{what} {}
};

class No_region_selected_exception : public Exception
{
public:
    No_region_selected_exception(const std::string& what) : Exception{what} {}
};

class Wrong_enum_value_exception : public Exception
{
public:
    Wrong_enum_value_exception(const std::string& what) : Exception{what} {}
};

class Websocket_exception : public Exception
{
public:
    Websocket_exception(const std::string& what) : Exception{what} {}
};

class Message_exception : public Exception
{
public:
    Message_exception(const std::string& what) : Exception{what} {}
};

} // namespace rito
