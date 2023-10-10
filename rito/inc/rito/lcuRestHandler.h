#pragma once

#include "commonTypes.h"
#include "lcuProcessHandler.h"

#include <memory>
#include <string>

namespace rito {

class HttpsRestClient;

class LcuRestHandler
{
public:
    LcuRestHandler();
    ~LcuRestHandler();

    HttpResponse request(RequestType type, const std::string& message);

private:
    void attemptReconnect();

private:
    std::unique_ptr<HttpsRestClient> m_client;
    LcuProcessHandler m_lcuProcessHandler;
};

} // namespace rito
