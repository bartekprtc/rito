#include "lcuWebsocketHandler.h"

#include "httpsWebsocketClient.h"
#include "lcuProcessHandler.h"

#include <chrono>
#include <iostream>
#include <stdexcept>

namespace rito {

using namespace std::literals::chrono_literals;

namespace {
constexpr auto EVENT_LOOP_DELAY{1s};
}

LcuWebsocketHandler::LcuWebsocketHandler() : m_isStateConnected{false}, m_stop{false} {}

LcuWebsocketHandler::~LcuWebsocketHandler()
{
    stop();
}

void LcuWebsocketHandler::run()
{
    m_stop = false;

    mainLoop();
}

void LcuWebsocketHandler::stop()
{
    m_stop = true;
    if (isConnected())
    {
        // Get out of receiveMessage function (any message will do)
        m_webSocket->sendMessage("Exit");
    }
}

void LcuWebsocketHandler::reconnect()
{
    std::lock_guard<std::mutex> socketLock{m_socketMutex};

    if (m_webSocket && !m_webSocket->isConnected())
    {
        m_webSocket.reset();
    }
    if (!m_webSocket)
    {
        LcuParameters lcuClientParameters{m_lcuProcessHandler.getLcuProcessParameters()};

        m_webSocket =
          std::make_unique<HttpsWebsocketClient>("127.0.0.1", lcuClientParameters.appPort);
        m_webSocket->setCredentials("riot", lcuClientParameters.remotingAuthToken);
        m_webSocket->start();
    }
}

bool LcuWebsocketHandler::isConnected()
{
    std::lock_guard<std::mutex> socketLock{m_socketMutex};
    return m_webSocket && m_webSocket->isConnected();
}

void LcuWebsocketHandler::notifyConnectionStateChanged(bool connected)
{
    if (connected && !m_isStateConnected)
    {
        if (m_onConnected)
        {
            m_onConnected();
        }
    }
    else if (!connected && m_isStateConnected)
    {
        if (m_onDisconnected)
        {
            m_onDisconnected();
        }
    }
    m_isStateConnected = connected;
}

void LcuWebsocketHandler::receiveMessage()
{
    try
    {
        auto res{m_webSocket->receiveMessage()};
        std::string response(res.begin(), res.end());
        if (m_onMessageReceived)
        {
            m_onMessageReceived(response);
        }
    }
    catch (...)
    {
    }
}

void LcuWebsocketHandler::reconnectAndNotify()
{
    notifyConnectionStateChanged(false);
    try
    {
        reconnect();
        notifyConnectionStateChanged(true);
    }
    catch (...)
    {
        std::this_thread::sleep_for(EVENT_LOOP_DELAY);
    }
}

void LcuWebsocketHandler::mainLoop()
{
    for (;;)
    {
        if (m_stop)
        {
            std::lock_guard<std::mutex> socketLock{m_socketMutex};
            m_webSocket.reset();
            notifyConnectionStateChanged(false);
            break;
        }

        if (isConnected())
        {
            receiveMessage();
        }
        else
        {
            reconnectAndNotify();
        }
    }
}

void LcuWebsocketHandler::registerMessageCallback(MessageCallback onMessageReceived)
{
    m_onMessageReceived = onMessageReceived;
}

void LcuWebsocketHandler::registerOnConnectedCallback(ConnectionCallback onConnected)
{
    m_onConnected = onConnected;
}

void LcuWebsocketHandler::registerOnDisconnectedCallback(ConnectionCallback onDisconnected)
{
    m_onDisconnected = onDisconnected;
}

bool LcuWebsocketHandler::sendMessage(std::string_view message)
{
    if (isConnected())
    {
        std::lock_guard<std::mutex> socketLock{m_socketMutex};
        return m_webSocket->sendMessage(message);
    }

    return false;
}

} // namespace rito
