#pragma once

#include "lcuProcessHandler.h"

#include <string>

namespace rito {

class LcuProcessHandlerImpl
{
public:
    LcuParameters getLcuProcessParameters();

private:
    std::string getLcuProcessCommand();
};

} // namespace rito
