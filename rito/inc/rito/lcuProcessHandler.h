#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace rito {

struct LcuParameters
{
    std::string remotingAuthToken;
    std::uint16_t appPort;
};

class LcuProcessHandlerImpl;

class LcuProcessHandler
{
public:
LcuProcessHandler();
~LcuProcessHandler();
   LcuParameters getLcuProcessParameters();

private:
    std::unique_ptr<LcuProcessHandlerImpl> m_pImpl;
};

} // namespace rito
