#pragma once

#include <stdexcept>
#include <string>

namespace rito {

class Exception : public std::runtime_error
{
public:
    Exception(const std::string& what) : std::runtime_error{what} {}
};

class UnknownException : public Exception
{
public:
    UnknownException(const std::string& what) : Exception{what} {}
};

class LcuNotRunningException : public Exception
{
public:
    LcuNotRunningException(const std::string& what) : Exception{what} {}
};

class LcuParametersException : public Exception
{
public:
    LcuParametersException(const std::string& what) : Exception{what} {}
};

class PathNotFoundException : public Exception
{
public:
    PathNotFoundException(const std::string& what) : Exception{what} {}
};

class NoRegionSelectedException : public Exception
{
public:
    NoRegionSelectedException(const std::string& what) : Exception{what} {}
};

class WrongEnumValueException : public Exception
{
public:
    WrongEnumValueException(const std::string& what) : Exception{what} {}
};

class WebSocketException : public Exception
{
public:
    WebSocketException(const std::string& what) : Exception{what} {}
};

class MessageException : public Exception
{
public:
    MessageException(const std::string& what) : Exception{what} {}
};


} // namespace rito
