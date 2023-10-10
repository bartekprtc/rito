#include "lcuProcessHandlerHelpers.h"
#include "exception.h"

#include <regex>

namespace rito {

std::string extractRemotingAuthToken(const std::string& lcuCommand)
{
    std::regex tokenRegex{"--remoting-auth-token=([a-zA-Z0-9-_]+)"};
    std::smatch tokenMatch;
    if (std::regex_search(lcuCommand, tokenMatch, tokenRegex))
    {
        return tokenMatch[1];
    }

    throw LcuParametersException(
      "Unable to extract remoting auth token from League Client launch command");
}

std::string extractAppPort(const std::string& lcuCommand)
{
    std::regex portRegex{"--app-port=(\\d+)"};
    std::smatch portMatch;
    if (std::regex_search(lcuCommand, portMatch, portRegex))
    {
        return portMatch[1];
    }

    throw LcuParametersException(
      "Unable to extract app port from League Client launch command");
}

bool isInteger(std::string_view str)
{
    return !str.empty() && std::all_of(str.cbegin(), str.cend(), [](char ch) {
        return std::isdigit(ch);
    });
}

}
