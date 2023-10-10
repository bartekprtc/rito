#pragma once

#include "commonTypes.h"
#include "lcuProcessHandler.h"

#include <memory>
#include <string_view>
#include <thread>
#include <mutex>

namespace rito {

class HttpsWebsocketClient;

class LcuWebsocketHandler
{
public:
    LcuWebsocketHandler();
    ~LcuWebsocketHandler();

    void run();
    void stop();

    void registerMessageCallback(MessageCallback onMessageReceived);
    void registerOnConnectedCallback(ConnectionCallback onConnected);
    void registerOnDisconnectedCallback(ConnectionCallback onDisconnected);

    bool sendMessage(std::string_view message);

private:
    void reconnect();
    bool isConnected();

    void notifyConnectionStateChanged(bool connected);
    void receiveMessage();
    void reconnectAndNotify();

    void mainLoop();

private:
    std::unique_ptr<HttpsWebsocketClient> m_webSocket;
    std::mutex m_socketMutex;

    bool m_isStateConnected;
    bool m_stop;

    MessageCallback m_onMessageReceived;
    ConnectionCallback m_onConnected;
    ConnectionCallback m_onDisconnected;

    LcuProcessHandler m_lcuProcessHandler;
};
} // namespace rito
