#include "lcuWampHandler.h"

#include <Poco/Dynamic/Var.h>
#include <Poco/Exception.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>

#include <tuple>

namespace rito {

using namespace Poco;
using namespace Poco::JSON;
using namespace Poco::Dynamic;

std::tuple<std::string, std::string> parseCallResult(JSON::Array::Ptr messageJsonArray)
{
    std::string callId{messageJsonArray->get(1).toString()};
    std::string callResult{messageJsonArray->get(2).toString()};

    return {callId, callResult};
}

std::tuple<std::string, std::string, std::string> parseCallError(JSON::Array::Ptr messageJsonArray)
{
    std::string callId{messageJsonArray->get(1).toString()};
    std::string error{messageJsonArray->get(2).toString()};
    std::string errorDescription{messageJsonArray->get(3).toString()};

    return {callId, error, errorDescription};
}

std::tuple<std::string, std::string> parseEvent(JSON::Array::Ptr messageJsonArray)
{
    std::string eventName{messageJsonArray->get(1).toString()};
    std::string eventData{messageJsonArray->get(2).toString()};

    return {eventName, eventData};
}

LcuWampHandler::LcuWampHandler() : m_isConnected{false}
{
    m_lcuWebsocketHandler.registerOnConnectedCallback(
      std::bind(&LcuWampHandler::onConnected, this));

    m_lcuWebsocketHandler.registerOnDisconnectedCallback(
      std::bind(&LcuWampHandler::onDisconnected, this));

    m_lcuWebsocketHandler.registerMessageCallback(
      std::bind(&LcuWampHandler::onMessage, this, std::placeholders::_1));
}

void LcuWampHandler::registerOnConnectedCallback(ConnectionCallback onConnected)
{
    m_onConnectedCallback = onConnected;
}

void LcuWampHandler::registerOnDisconnectedCallback(ConnectionCallback onDisconnected)
{
    m_onDisconnectedCallback = onDisconnected;
}

void LcuWampHandler::registerCallResultCallback(WampCallResultCallback onCallResult)
{
    m_onCallResultCallback = onCallResult;
}

void LcuWampHandler::registerCallErrorCallback(WampCallErrorCallback onCallError)
{
    m_onCallErrorCallback = onCallError;
}

void LcuWampHandler::registerEventCallback(WampEventCallback onEvent)
{
    m_onEventCallback = onEvent;
}

void LcuWampHandler::run()
{
    m_lcuWebsocketHandler.run();
}

bool LcuWampHandler::subscribe(const std::string& event)
{
    return m_lcuWebsocketHandler.sendMessage(
      std::format("[{}, \"{}\"]", static_cast<int>(WampMessageType::SUBSCRIBE), event));
}

bool LcuWampHandler::unsubscribe(const std::string& event)
{
    return m_lcuWebsocketHandler.sendMessage(
      std::format("[{}, \"{}\"]", static_cast<int>(WampMessageType::UNSUBSCRIBE), event));
}

void LcuWampHandler::onConnected()
{
    if (m_onConnectedCallback)
    {
        m_onConnectedCallback();
    }
}

void LcuWampHandler::onDisconnected()
{
    if (m_onDisconnectedCallback)
    {
        m_onDisconnectedCallback();
    }
}

void LcuWampHandler::onMessage(const std::string& message)
{
    if (message.empty())
    {
        return;
    }

    dispatchMessage(message);
}

void LcuWampHandler::dispatchMessage(const std::string& message)
{
    try
    {
        Parser jsonParser;

        const auto messageVar{jsonParser.parse(message)};
        const auto messageJsonArray{messageVar.extract<JSON::Array::Ptr>()};

        int wampMessageId{messageJsonArray->get(0)};
        switch (static_cast<int>(messageJsonArray->get(0)))
        {
            case static_cast<int>(WampMessageType::CALLRESULT):
            {
                const auto& [id, jsonData]{parseCallResult(messageJsonArray)};
                onCallResult(id, jsonData);
                break;
            }
            case static_cast<int>(WampMessageType::CALLERROR):
            {
                const auto& [id, error, errorDescription]{parseCallError(messageJsonArray)};
                onCallError(id, error, errorDescription);
                break;
            }
            case static_cast<int>(WampMessageType::EVENT):
            {
                const auto& [event, jsonData]{parseEvent(messageJsonArray)};
                onEvent(event, jsonData);
                break;
            }
        }
    }
    catch (Exception& e)
    {
    }
}

void LcuWampHandler::onEvent(const std::string& event, const std::string& jsonData)
{
    if (m_onEventCallback)
    {
        m_onEventCallback(event, jsonData);
    }
}

void LcuWampHandler::onCallResult(const std::string& id, const std::string& jsonData)
{
    if (m_onCallResultCallback)
    {
        m_onCallResultCallback(id, jsonData);
    }
}

void LcuWampHandler::onCallError(const std::string& id,
                                 const std::string& error,
                                 const std::string& errorDescription)
{
    if (m_onCallErrorCallback)
    {
        m_onCallErrorCallback(id, error, errorDescription);
    }
}

} // namespace rito
