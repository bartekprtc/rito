#pragma once

#include <functional>
#include <string>
#include <cstdint>

namespace rito {

// Arg1: message (JSON)
using MessageCallback = std::function<void(const std::string&)>;

using ConnectionCallback = std::function<void()>;

// Arg1: ID, Arg2: data (JSON)
using WampCallResultCallback = std::function<void(const std::string&, const std::string&)>;

// Arg1: ID, Arg2: error type, Arg3: error description
using WampCallErrorCallback =
  std::function<void(const std::string&, const std::string&, const std::string&)>;

// Arg1: event, Arg2: data (JSON)
using WampEventCallback = std::function<void(const std::string&, const std::string&)>;

enum class RequestType
{
    CONNECT,
    DELETE,
    GET,
    HEAD,
    OPTIONS,
    PATCH,
    POST,
    PUT,
    TRACE
};

struct HttpResponse
{
    std::int32_t status;
    std::string reason;
    std::string response;
};

}
