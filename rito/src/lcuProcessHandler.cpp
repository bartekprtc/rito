#include "lcuProcessHandler.h"

#include "lcuProcessHandlerImpl.h"

namespace rito {

LcuProcessHandler::LcuProcessHandler()
: m_pImpl{std::make_unique<LcuProcessHandlerImpl>()}
{
}

LcuProcessHandler::~LcuProcessHandler()
{
}

LcuParameters LcuProcessHandler::getLcuProcessParameters()
{
    return m_pImpl->getLcuProcessParameters();
}


} // namespace rito
