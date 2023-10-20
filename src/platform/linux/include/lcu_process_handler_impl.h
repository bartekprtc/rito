#pragma once

#include "lcu_process_handler.h"

#include <string>
#include <filesystem>

namespace rito {

class Lcu_process_handler_impl
{
public:
    Lcu_process_handler_impl(std::filesystem::path proc_dir);
    Lcu_parameters get_lcu_process_parameters();

private:
    std::string get_lcu_process_command();

private:
    std::filesystem::path m_proc_dir;
};

} // namespace rito
