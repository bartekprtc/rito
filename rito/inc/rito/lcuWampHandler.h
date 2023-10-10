#pragma once

#include "commonTypes.h"
#include "lcuWebsocketHandler.h"

#include <concepts>
#include <format>
#include <iostream>
#include <string>
#include <string_view>

namespace rito {

using namespace std::literals::string_literals;

template <class T>
concept ConvertibleToString = std::convertible_to<T, std::string>;

enum class WampMessageType
{
    CALL = 2,
    CALLRESULT = 3,
    CALLERROR = 4,
    SUBSCRIBE = 5,
    UNSUBSCRIBE = 6,
    EVENT = 8
};

class LcuWampHandler
{
public:
    LcuWampHandler();

    void registerOnConnectedCallback(ConnectionCallback onConnected);
    void registerOnDisconnectedCallback(ConnectionCallback onDisconnected);
    void registerCallResultCallback(WampCallResultCallback onCallResult);
    void registerCallErrorCallback(WampCallErrorCallback onCallError);
    void registerEventCallback(WampEventCallback onEvent);

    void run();

    template <ConvertibleToString... Args>
    bool call(const std::string& id, const std::string& function, Args... args);
    bool subscribe(const std::string& event);
    bool unsubscribe(const std::string& event);

private:
    void onConnected();
    void onDisconnected();

    void onMessage(const std::string& message);
    void dispatchMessage(const std::string& message);

    void onEvent(const std::string& event, const std::string& jsonData);
    void onCallResult(const std::string& id, const std::string& jsonData);
    void onCallError(const std::string& id,
                     const std::string& error,
                     const std::string& errorDescription);

private:
    bool m_isConnected;

    LcuWebsocketHandler m_lcuWebsocketHandler;

    ConnectionCallback m_onConnectedCallback;
    ConnectionCallback m_onDisconnectedCallback;
    WampCallResultCallback m_onCallResultCallback;
    WampCallErrorCallback m_onCallErrorCallback;
    WampEventCallback m_onEventCallback;
};

template <ConvertibleToString... Args>
bool LcuWampHandler::call(const std::string& id, const std::string& function, Args... args)
{
    std::string arguments;
    for (const auto& arg : {args...})
    {
        arguments += ", \""s + arg + "\""s;
    }

    return m_lcuWebsocketHandler.sendMessage(std::format(
      "[{}, \"{}\", \"{}\"{}]", static_cast<int>(WampMessageType::CALL), id, function, arguments));
}

} // namespace rito
