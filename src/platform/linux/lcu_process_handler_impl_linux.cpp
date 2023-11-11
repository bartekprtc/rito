#include "lcu_process_handler_impl.h"

#include "exception.h"
#include "lcu_process_handler_helpers.h"

#include <cctype>
#include <filesystem>
#include <fstream>

using namespace std::literals::string_literals;

namespace rito {

Lcu_process_handler_impl::Lcu_process_handler_impl(std::filesystem::path proc_dir)
  : m_proc_dir{std::move(proc_dir)}
{
}

auto Lcu_process_handler_impl::get_lcu_process_parameters() -> Lcu_parameters
{
    std::string lcu_command{get_lcu_process_command()};

    std::string remoting_auth_token{extract_remoting_auth_token(lcu_command)};
    std::string app_port{extract_app_port(lcu_command)};

    return {remoting_auth_token, static_cast<uint16_t>(std::stoi(app_port))};
}

auto Lcu_process_handler_impl::get_lcu_process_command() -> std::string
{
    try
    {
        for (const auto& dir : std::filesystem::directory_iterator(m_proc_dir))
        {
            std::string_view proc_id{dir.path().stem().c_str()};
            if (!is_positive_integer(proc_id))
            {
                continue;
            }

            std::ifstream cmdline_file{dir.path().string() + "/cmdline"};
            std::string command;
            std::getline(cmdline_file, command);
            if (command.find("LeagueClientUx.exe") != std::string::npos)
            {
                return command;
            }
        }
    }
    catch (std::filesystem::filesystem_error& e)
    {
        throw Path_not_found_exception{"Unable to locate "s + m_proc_dir.string() + " folder"};
    }

    throw Lcu_not_running_exception("Unable to find League Client process");
}

} // namespace rito
