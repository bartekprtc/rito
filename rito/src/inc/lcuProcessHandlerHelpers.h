#pragma once

#include <string>
#include <string_view>

namespace rito {

std::string extractRemotingAuthToken(const std::string& lcuCommand);
std::string extractAppPort(const std::string& lcuCommand);
bool isInteger(std::string_view str);

}
