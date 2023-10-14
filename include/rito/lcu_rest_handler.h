#pragma once

#include "rito_export.h"

#include "common_types.h"
#include "lcu_process_handler.h"

#include <memory>
#include <string>

namespace rito {

class Https_rest_client;

class RITO_EXPORT Lcu_rest_handler
{
public:
    Lcu_rest_handler();
    ~Lcu_rest_handler();

    Http_response request(Request_type type, const std::string& message);

private:
    void attempt_reconnect();

private:
    std::unique_ptr<Https_rest_client> m_client;
    Lcu_process_handler m_lcu_process_handler;
};

} // namespace rito
