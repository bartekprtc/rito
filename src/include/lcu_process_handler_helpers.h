#pragma once

#include <string>
#include <string_view>

namespace rito {

auto extract_remoting_auth_token(const std::string& lcu_command) -> std::string;
auto extract_app_port(const std::string& lcu_command) -> std::string;
auto is_positive_integer(std::string_view str) -> bool;

}
