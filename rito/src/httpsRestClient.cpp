#include "httpsRestClient.h"

#include "exception.h"

#include <Poco/Net/HTTPMessage.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/NetException.h>

#include <format>
#include <iostream>

namespace rito {

using Poco::Net::Context;
using Poco::Net::HTTPMessage;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;

HttpsRestClient::HttpsRestClient(const std::string& host, std::uint16_t port)
  : m_host{host},
    m_port{port},
    m_context{new Context(Context::TLS_CLIENT_USE, "", "", "", Context::VERIFY_NONE)},
    m_session{m_host, m_port, m_context}
{
}

HttpResponse HttpsRestClient::request(RequestType type,
                                      const std::string& pathAndQuery,
                                      const KeyValueMap& headers)
{
    sendRequest(toPocoRequestType(type), pathAndQuery, headers);

    return receiveResponse();
}

void HttpsRestClient::sendRequest(const std::string& type,
                                  const std::string& pathAndQuery,
                                  const KeyValueMap& headers)
{
    HTTPRequest request(type, pathAndQuery, HTTPMessage::HTTP_1_1);
    request.setHost(m_host, m_port);
    request.setKeepAlive(true);

    if (!m_credentials.empty())
    {
        m_credentials.authenticate(request);
    }

    for (const auto& [key, value] : headers)
    {
        request.set(key, value);
    }

    m_session.sendRequest(request);
}

HttpResponse HttpsRestClient::receiveResponse()
{
    try
    {
        HTTPResponse response;
        auto& responseStream{m_session.receiveResponse(response)};

        HttpResponse res{.status = response.getStatus(), .reason = response.getReason()};
        responseStream >> res.response;

        return res;
    }
    catch (Poco::Net::MessageException& e)
    {
        throw MessageException{
          std::format("Unable to receive message from the server({})", e.what())};
    }

    throw UnknownException{"HttpsRestClient: Unexpected error while receiving message"};
}

std::string toPocoRequestType(RequestType type)
{
    switch (type)
    {
        case RequestType::CONNECT:
            return HTTPRequest::HTTP_CONNECT;
        case RequestType::DELETE:
            return HTTPRequest::HTTP_DELETE;
        case RequestType::GET:
            return HTTPRequest::HTTP_GET;
        case RequestType::HEAD:
            return HTTPRequest::HTTP_HEAD;
        case RequestType::OPTIONS:
            return HTTPRequest::HTTP_OPTIONS;
        case RequestType::PATCH:
            return HTTPRequest::HTTP_PATCH;
        case RequestType::POST:
            return HTTPRequest::HTTP_POST;
        case RequestType::PUT:
            return HTTPRequest::HTTP_PUT;
        case RequestType::TRACE:
            return HTTPRequest::HTTP_TRACE;
    }

    throw WrongEnumValueException{"Unknown RequestType enum value"};
}

} // namespace rito
