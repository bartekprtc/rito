#include "lcu_process_handler_helpers.h"
#include "exception.h"

#include <regex>

namespace rito {

auto extract_remoting_auth_token(const std::string& lcu_command) -> std::string
{
    std::regex token_regex{"--remoting-auth-token=([a-zA-Z0-9-_]+)[^a-zA-Z0-9-_]"};
    std::smatch token_match;
    if (std::regex_search(lcu_command, token_match, token_regex))
    {
        return token_match[1];
    }

    throw Lcu_parameters_exception(
      "Unable to extract remoting auth token from League Client launch command");
}

auto extract_app_port(const std::string& lcu_command) -> std::string
{
    std::regex port_regex{"--app-port=(\\d+)\\D"};
    std::smatch port_match;
    if (std::regex_search(lcu_command, port_match, port_regex))
    {
        return port_match[1];
    }

    throw Lcu_parameters_exception("Unable to extract app port from League Client launch command");
}

auto is_positive_integer(std::string_view str) -> bool
{
    return !str.empty() && std::all_of(str.cbegin(), str.cend(), [](char character) {
        return std::isdigit(character);
    });
}

} // namespace rito
