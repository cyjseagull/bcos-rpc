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
 *  limitations under the License.
 *
 * @file AMOP.h
 * @author: octopus
 * @date 2021-06-21
 */
#pragma once

#include <bcos-rpc/amop/AMOPInterface.h>
#include <bcos-rpc/amop/AMOPMessage.h>
#include <bcos-rpc/amop/TopicManager.h>
#include <boost/asio.hpp>

namespace bcos
{
namespace amop
{
class AMOP : public AMOPInterface, public std::enable_shared_from_this<AMOP>
{
public:
    using Ptr = std::shared_ptr<AMOP>;

public:
    AMOP() = default;
    virtual ~AMOP() { stop(); }

    void initMsgHandler();

public:
    virtual void start() override;
    virtual void stop() override;

    /**
     * @brief: async receive message from front service
     * @param _nodeID: the message sender nodeID
     * @param _id: the id of this message, it can by used to send response to the peer
     * @param _data: the message data
     * @return void
     */
    virtual void asyncNotifyAmopMessage(bcos::crypto::NodeIDPtr _nodeID, const std::string& _id,
        bcos::bytesConstRef _data) override;
    /**
     * @brief: async receive nodeIDs from front service
     * @param _nodeIDs: the nodeIDs
     * @param _callback: callback
     * @return void
     */
    virtual void asyncNotifyAmopNodeIDs(std::shared_ptr<const crypto::NodeIDs> _nodeIDs,
        std::function<void(bcos::Error::Ptr _error)> _callback) override;

public:
    /**
     * @brief: create message and encode the message to bytes
     * @param _type: message type
     * @param _data: message data
     * @return std::shared_ptr<bytes>
     */
    std::shared_ptr<bytes> buildEncodedMessage(uint32_t _type, bcos::bytesConstRef _data);
    /**
     * @brief: periodically send topicSeq to all other nodes
     * @return void
     */
    void broadcastTopicSeq();
    /**
     * @brief: receive topicSeq from other nodes
     * @param _nodeID: the sender nodeID
     * @param _id: the message id
     * @param _data: message data
     * @return void
     */
    void onReceiveTopicSeqMessage(
        bcos::crypto::NodeIDPtr _nodeID, const std::string& _id, bcos::bytesConstRef _data);
    /**
     * @brief: receive request topic message from other nodes
     * @param _nodeID: the sender nodeID
     * @param _id: the message id
     * @param _data: message data
     * @return void
     */
    void onReceiveRequestTopicMessage(
        bcos::crypto::NodeIDPtr _nodeID, const std::string& _id, bcos::bytesConstRef _data);
    /**
     * @brief: receive topic response message from other nodes
     * @param _nodeID: the sender nodeID
     * @param _id: the message id
     * @param _data: message data
     * @return void
     */
    void onReceiveResponseTopicMessage(
        bcos::crypto::NodeIDPtr _nodeID, const std::string& _id, bcos::bytesConstRef _data);

public:
    std::shared_ptr<boost::asio::io_service> ioService() const { return m_ioService; }
    void setIoService(const std::shared_ptr<boost::asio::io_service> _ioService)
    {
        m_ioService = _ioService;
    }
    std::shared_ptr<MessageFactory> messageFactory() const { return m_messageFactory; }
    void setMessageFactory(std::shared_ptr<MessageFactory> _messageFactory)
    {
        m_messageFactory = _messageFactory;
    }
    std::shared_ptr<TopicManager> topicManager() const { return m_topicManager; }
    void setTopicManager(std::shared_ptr<TopicManager> _topicManager)
    {
        m_topicManager = _topicManager;
    }

    std::shared_ptr<bcos::front::FrontServiceInterface> frontServiceInterface() const
    {
        return m_frontServiceInterface;
    }
    void setFrontServiceInterface(
        std::shared_ptr<bcos::front::FrontServiceInterface> _frontServiceInterface)
    {
        m_frontServiceInterface = _frontServiceInterface;
    }

private:
    bool m_run = false;
    std::shared_ptr<bcos::front::FrontServiceInterface> m_frontServiceInterface;
    std::shared_ptr<MessageFactory> m_messageFactory;
    std::shared_ptr<TopicManager> m_topicManager;

    std::shared_ptr<boost::asio::deadline_timer> m_timer;
    std::shared_ptr<boost::asio::io_service> m_ioService;

    std::unordered_map<uint16_t, std::function<void(bcos::crypto::NodeIDPtr _nodeID,
                                     const std::string& _id, bcos::bytesConstRef _data)>>
        m_msgTypeToHandler;
};
}  // namespace amop
}  // namespace bcos