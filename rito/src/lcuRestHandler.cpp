#include "lcuRestHandler.h"

#include "exception.h"
#include "httpsRestClient.h"
#include "lcuProcessHandler.h"

#include <iostream>
#include <memory>

namespace rito {

LcuRestHandler::LcuRestHandler() {}

LcuRestHandler::~LcuRestHandler() {}

HttpResponse LcuRestHandler::request(RequestType type, const std::string& message)
{
    if (m_client)
    {
        try
        {
            return m_client->request(type, message);
        }
        catch (...)
        {
            m_client.reset();
        }
    }

    if (!m_client)
    {
        attemptReconnect();
        return m_client->request(type, message);
    }

    throw UnknownException{"Unexpected error"};
}

void LcuRestHandler::attemptReconnect()
{
    LcuParameters lcuClientParameters{m_lcuProcessHandler.getLcuProcessParameters()};
    m_client = std::make_unique<HttpsRestClient>("127.0.0.1", lcuClientParameters.appPort);
    m_client->setCredentials("riot", lcuClientParameters.remotingAuthToken);
}

} // namespace rito
