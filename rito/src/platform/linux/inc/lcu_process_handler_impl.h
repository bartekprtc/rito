#pragma once

#include "lcu_process_handler.h"

#include <string>

namespace rito {

class Lcu_process_handler_impl
{
public:
    Lcu_parameters get_lcu_process_parameters();

private:
    std::string get_lcu_process_command();
};

} // namespace rito
