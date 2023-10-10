#include <Poco/Foundation.h>

#if defined(POCO_OS_FAMILY_UNIX)

  #include "lcuProcessHandlerImpl.h"

  #include "exception.h"
  #include "lcuProcessHandlerHelpers.h"

  #include <cctype>
  #include <filesystem>
  #include <fstream>

namespace rito {

LcuParameters LcuProcessHandlerImpl::getLcuProcessParameters()
{
    std::string lcuCommand{getLcuProcessCommand()};

    std::string remotingAuthToken{extractRemotingAuthToken(lcuCommand)};
    std::string appPort{extractAppPort(lcuCommand)};

    return {remotingAuthToken, static_cast<uint16_t>(std::stoi(appPort))};
}

std::string LcuProcessHandlerImpl::getLcuProcessCommand()
{
    try
    {
        std::filesystem::path procDir("/proc");
        for (const auto& dir : std::filesystem::directory_iterator(procDir))
        {
            std::string_view procId{dir.path().stem().c_str()};
            if (!isInteger(procId))
            {
                continue;
            }

            std::ifstream cmdlineFile{dir.path().string() + "/cmdline"};
            std::string command;
            std::getline(cmdlineFile, command);
            if (command.find("LeagueClientUx.exe") != std::string::npos)
            {
                return command;
            }
        }
    }
    catch (std::filesystem::filesystem_error& e)
    {
        throw PathNotFoundException{"Unable to locate /proc folder"};
    }

    throw LcuNotRunningException("Unable to find League Client process");
}

} // namespace rito

#endif
