#pragma once

#include "rito_export.h"

#include <cstdint>
#include <functional>
#include <string>

namespace rito {

// Arg1: message (JSON)
using Message_callback = std::function<void(const std::string&)>;

using Connection_callback = std::function<void()>;

// Arg1: ID, Arg2: data (JSON)
using Wamp_call_result_callback = std::function<void(const std::string&, const std::string&)>;

// Arg1: ID, Arg2: error type, Arg3: error description
using Wamp_call_error_callback =
  std::function<void(const std::string&, const std::string&, const std::string&)>;

// Arg1: event, Arg2: data (JSON)
using Wamp_event_callback = std::function<void(const std::string&, const std::string&)>;

enum class RITO_EXPORT Request_type
{
    http_connect,
    http_delete,
    http_get,
    http_head,
    http_options,
    http_patch,
    http_post,
    http_put,
    http_trace
};

struct RITO_EXPORT Http_response
{
    std::int32_t status;
    std::string reason;
    std::string response;
};

} // namespace rito
