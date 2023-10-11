#include "lcu_rest_handler.h"

#include "exception.h"
#include "https_rest_client.h"
#include "lcu_process_handler.h"

#include <iostream>
#include <memory>

namespace rito {

Lcu_rest_handler::Lcu_rest_handler() {}

Lcu_rest_handler::~Lcu_rest_handler() {}

Http_response Lcu_rest_handler::request(Request_type type, const std::string& message)
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
        attempt_reconnect();
        return m_client->request(type, message);
    }

    throw Unknown_exception{"Unexpected error"};
}

void Lcu_rest_handler::attempt_reconnect()
{
    Lcu_parameters lcu_client_parameters{m_lcu_process_handler.get_lcu_process_parameters()};
    m_client = std::make_unique<Https_rest_client>("127.0.0.1", lcu_client_parameters.app_port);
    m_client->set_credentials("riot", lcu_client_parameters.remoting_auth_token);
}

} // namespace rito
