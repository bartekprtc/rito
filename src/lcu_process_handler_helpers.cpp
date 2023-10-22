#include "lcu_process_handler_helpers.h"
#include "exception.h"

#include <regex>

namespace rito {

std::string extract_remoting_auth_token(const std::string& lcu_command)
{
    std::regex token_regex{"--remoting-auth-token=([a-zA-Z0-9-_]+)\\s"};
    std::smatch token_match;
    if (std::regex_search(lcu_command, token_match, token_regex))
    {
        return token_match[1];
    }

    throw Lcu_parameters_exception(
      "Unable to extract remoting auth token from League Client launch command");
}

std::string extract_app_port(const std::string& lcu_command)
{
    std::regex port_regex{"--app-port=(\\d+)\\s"};
    std::smatch port_match;
    if (std::regex_search(lcu_command, port_match, port_regex))
    {
        return port_match[1];
    }

    throw Lcu_parameters_exception("Unable to extract app port from League Client launch command");
}

bool is_integer(std::string_view str)
{
    return !str.empty() && std::all_of(str.cbegin(), str.cend(), [](char ch) {
        return std::isdigit(ch);
    });
}

} // namespace rito
