#pragma once

#include "rito_export.h"

#include <cstdint>
#include <memory>
#include <string>

namespace rito {

struct RITO_EXPORT Lcu_parameters
{
    std::string remoting_auth_token;
    std::uint16_t app_port;
};

class Lcu_process_handler_impl;

class RITO_EXPORT Lcu_process_handler
{
public:
    Lcu_process_handler();
    ~Lcu_process_handler();
    Lcu_parameters get_lcu_process_parameters();

private:
    std::unique_ptr<Lcu_process_handler_impl> m_pimpl;
};

} // namespace rito
