#include "https_rest_client.h"

#include "exception.h"

#include <Poco/Net/HTTPMessage.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/NetException.h>

#include <iostream>
#include <iterator>

using namespace std::literals::string_literals;

namespace rito {

using Poco::Net::Context;
using Poco::Net::HTTPMessage;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;

Https_rest_client::Https_rest_client(std::string host, std::uint16_t port)
  : m_host{std::move(host)},
    m_port{port},
    m_context{new Context(Context::TLS_CLIENT_USE, "", "", "", Context::VERIFY_NONE)},
    m_session{m_host, m_port, m_context}
{
}

auto Https_rest_client::request(Request_type type,
                                const std::string& path_and_query,
                                const Key_value_map& headers) -> Http_response
{
    send_request(to_poco_request_type(type), path_and_query, headers);

    return receive_response();
}

void Https_rest_client::send_request(const std::string& type,
                                     const std::string& path_and_query,
                                     const Key_value_map& headers)
{
    HTTPRequest request(type, path_and_query, HTTPMessage::HTTP_1_1);
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

auto Https_rest_client::receive_response() -> Http_response
{
    try
    {
        HTTPResponse response;
        auto& response_stream{m_session.receiveResponse(response)};

        Http_response res{.status = response.getStatus(), .reason = response.getReason()};
        res.response = std::string(std::istreambuf_iterator<char>(response_stream), {});

        return res;
    }
    catch (Poco::Net::MessageException& e)
    {
        throw Message_exception{"Unable to receive message from the server ("s + e.what() + ")"};
    }
}

auto to_poco_request_type(Request_type type) -> std::string
{
    switch (type)
    {
        case Request_type::http_connect:
            return HTTPRequest::HTTP_CONNECT;
        case Request_type::http_delete:
            return HTTPRequest::HTTP_DELETE;
        case Request_type::http_get:
            return HTTPRequest::HTTP_GET;
        case Request_type::http_head:
            return HTTPRequest::HTTP_HEAD;
        case Request_type::http_options:
            return HTTPRequest::HTTP_OPTIONS;
        case Request_type::http_patch:
            return HTTPRequest::HTTP_PATCH;
        case Request_type::http_post:
            return HTTPRequest::HTTP_POST;
        case Request_type::http_put:
            return HTTPRequest::HTTP_PUT;
        case Request_type::http_trace:
            return HTTPRequest::HTTP_TRACE;
    }

    throw Wrong_enum_value_exception{"Unknown RequestType enum value"};
}

} // namespace rito
