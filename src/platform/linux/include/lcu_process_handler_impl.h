#pragma once

#include "lcu_process_handler.h"

#include <string>
#include <filesystem>

namespace rito {

class Lcu_process_handler_impl
{
public:
    Lcu_process_handler_impl(std::filesystem::path proc_dir);
    auto get_lcu_process_parameters() -> Lcu_parameters;

private:
    auto get_lcu_process_command() -> std::string;

    std::filesystem::path m_proc_dir;
};

} // namespace rito
