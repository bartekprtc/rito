#pragma once

#include <string>
#include <string_view>

namespace rito {

std::string extract_remoting_auth_token(const std::string& lcu_command);
std::string extract_app_port(const std::string& lcu_command);
bool is_integer(std::string_view str);

}
