/*
 *  Copyright (C) 2021 FISCO BCOS.
 *  SPDX-License-Identifier: Apache-2.0
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  m_limitations under the License.
 *
 * @file WsSession.h
 * @author: octopus
 * @date 2021-07-28
 */
#pragma once
#include "libutilities/Common.h"
#include <bcos-rpc/http/Common.h>
#include <bcos-rpc/http/ws/Common.h>
#include <bcos-rpc/http/ws/WsMessage.h>
#include <bcos-rpc/http/ws/WsVersion.h>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <atomic>
#include <iostream>
#include <memory>
#include <shared_mutex>
#include <unordered_map>

namespace bcos
{
class ThreadPool;
namespace ws
{
class WsService;

// The websocket session for connection
class WsSession : public std::enable_shared_from_this<WsSession>
{
public:
    using Ptr = std::shared_ptr<WsSession>;

public:
    WsSession(boost::asio::ip::tcp::socket&& socket) : m_wsStream(std::move(socket))
    {
        WEBSOCKET_SESSION(INFO) << LOG_KV("[NEWOBJ][WSSESSION]", this);
    }

    virtual ~WsSession()
    {
        disconnect();
        WEBSOCKET_SESSION(INFO) << LOG_KV("[DELOBJ][WSSESSION]", this);
    }

    void drop();
    void disconnect();

public:
    // start websocket accept
    void doAccept(http::HttpRequest _req);
    void onAccept(boost::beast::error_code _ec);
    // async read
    void onRead(boost::beast::error_code _ec, std::size_t);
    void asyncRead();
    // async write
    void onWrite(boost::beast::error_code _ec, std::size_t);
    void asyncWrite();

    /**
     * @brief: async send message
     * @param _msg: message
     * @param _options: options
     * @param _respCallback: callback
     * @return void:
     */
    void asyncSendMessage(std::shared_ptr<WsMessage> _msg, Options _options = Options(-1),
        RespCallBack _respCallback = RespCallBack());

public:
    bool isConnected() { return m_wsStream.next_layer().socket().is_open() && !m_isDrop; }

    std::string remoteEndPoint() const { return m_remoteEndPoint; }
    std::string localEndPoint() const { return m_localEndPoint; }

    void setAcceptHandler(WsAcceptHandler _acceptHandler) { m_acceptHandler = _acceptHandler; }
    WsAcceptHandler acceptHandler() { return m_acceptHandler; }

    void setDisconnectHandler(WsDisconnectHandler _disconnectHandler)
    {
        m_disconnectHandler = _disconnectHandler;
    }
    WsDisconnectHandler disconnectHandler() { return m_disconnectHandler; }

    void setRecvMessageHandler(WsRecvMessageHandler _recvMessageHandler)
    {
        m_recvMessageHandler = _recvMessageHandler;
    }
    WsRecvMessageHandler recvMessageHandler() { return m_recvMessageHandler; }

    std::shared_ptr<WsMessageFactory> messageFactory() { return m_messageFactory; }
    void setMessageFactory(std::shared_ptr<WsMessageFactory> _messageFactory)
    {
        m_messageFactory = _messageFactory;
    }

    std::shared_ptr<bcos::ThreadPool> threadPool() const { return m_threadPool; }
    void setThreadPool(std::shared_ptr<bcos::ThreadPool> _threadPool)
    {
        m_threadPool = _threadPool;
    }

    void setVersion(uint16_t _version) { m_version = _version; }
    uint16_t version() const { return m_version; }

    struct CallBack
    {
        using Ptr = std::shared_ptr<CallBack>;
        RespCallBack respCallBack;
        std::shared_ptr<boost::asio::deadline_timer> timer;
    };
    void addRespCallback(const std::string& _seq, CallBack::Ptr _callback);
    CallBack::Ptr getAndRemoveRespCallback(const std::string& _seq);
    void onRespTimeout(const boost::system::error_code& _error, const std::string& _seq);

private:
    // websocket protocol version
    uint16_t m_version = WsProtocolVersion::current;

    // buffer used to read message
    boost::beast::flat_buffer m_buffer;
    //
    std::atomic_bool m_isDrop = false;
    // websocket stream
    boost::beast::websocket::stream<boost::beast::tcp_stream> m_wsStream;
    std::string m_remoteEndPoint;
    std::string m_localEndPoint;

    // callbacks
    mutable std::shared_mutex x_callback;
    std::unordered_map<std::string, CallBack::Ptr> m_callbacks;

    // callback handler
    WsDisconnectHandler m_disconnectHandler;
    WsAcceptHandler m_acceptHandler;
    WsRecvMessageHandler m_recvMessageHandler;

    // message factory
    std::shared_ptr<WsMessageFactory> m_messageFactory;
    // thread pool
    std::shared_ptr<bcos::ThreadPool> m_threadPool;

    // send message queue
    mutable std::shared_mutex x_queue;
    std::vector<std::shared_ptr<bcos::bytes>> m_queue;
};

}  // namespace ws
}  // namespace bcos