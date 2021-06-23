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
 * @file AMOP.cpp
 * @author: octopus
 * @date 2021-06-21
 */

#include <bcos-framework/interfaces/front/FrontServiceInterface.h>
#include <bcos-framework/interfaces/protocol/CommonError.h>
#include <bcos-framework/interfaces/protocol/Protocol.h>
#include <bcos-framework/libutilities/DataConvertUtility.h>
#include <bcos-framework/libutilities/Exceptions.h>
#include <bcos-rpc/amop/AMOP.h>
#include <bcos-rpc/amop/AMOPMessage.h>
#include <bcos-rpc/amop/Common.h>

using namespace bcos;
using namespace bcos::amop;

void AMOP::initMsgHandler()
{
    auto self = std::weak_ptr<AMOP>(shared_from_this());
    m_msgTypeToHandler[AMOPMessageType::TopicSeq] =
        [self](bcos::crypto::NodeIDPtr _nodeID, const std::string& _id, bcos::bytesConstRef _data) {
            auto amop = self.lock();
            if (amop)
            {
                amop->onReceiveTopicSeqMessage(_nodeID, _id, _data);
            }
        };

    m_msgTypeToHandler[AMOPMessageType::RequestTopic] =
        [self](bcos::crypto::NodeIDPtr _nodeID, const std::string& _id, bcos::bytesConstRef _data) {
            auto amop = self.lock();
            if (amop)
            {
                amop->onReceiveRequestTopicMessage(_nodeID, _id, _data);
            }
        };

    m_msgTypeToHandler[AMOPMessageType::ResponseTopic] =
        [self](bcos::crypto::NodeIDPtr _nodeID, const std::string& _id, bcos::bytesConstRef _data) {
            auto amop = self.lock();
            if (amop)
            {
                amop->onReceiveResponseTopicMessage(_nodeID, _id, _data);
            }
        };

    // m_msgTypeToHandler[AMOPMessageType::AMOPRequest] = nullptr;
    // m_msgTypeToHandler[AMOPMessageType::AMOPResponse] = nullptr;
    // m_msgTypeToHandler[AMOPMessageType::AMOPBroadcast] = nullptr;
}

void AMOP::start()
{
    if (!m_frontServiceInterface)
    {
        BOOST_THROW_EXCEPTION(
            InvalidParameter() << errinfo_comment("AMOP::start front service is uninitialized"));
    }

    if (!m_topicManager)
    {
        BOOST_THROW_EXCEPTION(
            InvalidParameter() << errinfo_comment("AMOP::start topic manager is uninitialized"));
    }

    if (!m_messageFactory)
    {
        BOOST_THROW_EXCEPTION(
            InvalidParameter() << errinfo_comment("AMOP::start message factory is uninitialized"));
    }

    if (m_run)
    {
        AMOP_LOG(INFO) << LOG_DESC("amop is running");
        return;
    }

    m_run = true;
    // init message handler
    initMsgHandler();
    // broadcast topic seq periodically
    broadcastTopicSeq();
    AMOP_LOG(INFO) << LOG_DESC("start amop successfully");
}

void AMOP::stop()
{
    if (!m_run)
    {
        AMOP_LOG(INFO) << LOG_DESC("amop is not running");
        return;
    }

    m_run = false;
    m_msgTypeToHandler.clear();

    if (m_timer)
    {
        m_timer->cancel();
    }

    if (m_ioService)
    {
        m_ioService->stop();
    }

    AMOP_LOG(INFO) << LOG_DESC("stop amop successfully");
}

/**
 * @brief: create message and encode the message to bytes
 * @param _type: message type
 * @param _data: message data
 * @return std::shared_ptr<bytes>
 */
std::shared_ptr<bytes> AMOP::buildEncodedMessage(uint32_t _type, bcos::bytesConstRef _data)
{
    auto message = m_messageFactory->buildMessage(_type, _data);
    auto buffer = std::make_shared<bytes>();
    message->encode(*buffer.get());
    return buffer;
}

/**
 * @brief: periodically send topicSeq to all other nodes
 * @return void
 */
void AMOP::broadcastTopicSeq()
{
    auto topicSeq = std::to_string(m_topicManager->topicSeq());
    auto buffer = buildEncodedMessage(
        AMOPMessageType::TopicSeq, bytesConstRef((byte*)topicSeq.data(), topicSeq.size()));
    m_frontServiceInterface->asyncSendBroadcastMessage(
        bcos::protocol::ModuleID::AMOP, bytesConstRef(buffer->data(), buffer->size()));

    AMOP_LOG(DEBUG) << LOG_DESC("broadcastTopicSeq") << LOG_KV("topicSeq", topicSeq);

    auto self = std::weak_ptr<AMOP>(shared_from_this());
    m_timer = std::make_shared<boost::asio::deadline_timer>(
        *m_ioService, boost::posix_time::milliseconds(2000));
    m_timer->async_wait([self](boost::system::error_code e) {
        if (e)
        {
            return;
        }

        auto s = self.lock();
        if (s)
        {
            s->broadcastTopicSeq();
        }
    });
}

/**
 * @brief: receive topicSeq from other nodes
 * @param _nodeID: the sender nodeID
 * @param _id: the message id
 * @param _data: message data
 * @return void
 */
void AMOP::onReceiveTopicSeqMessage(
    bcos::crypto::NodeIDPtr _nodeID, const std::string& _id, bcos::bytesConstRef _data)
{
    try
    {
        uint32_t topicSeq = boost::lexical_cast<uint32_t>(std::string(_data.begin(), _data.end()));
        if (!m_topicManager->checkTopicSeq(_nodeID->hex(), topicSeq))
        {
            AMOP_LOG(TRACE) << LOG_DESC("onReceiveTopicSeqMessage")
                            << LOG_KV("nodeID", _nodeID->hex()) << LOG_KV("id", _id)
                            << LOG_KV("topicSeq", topicSeq);
            return;
        }

        AMOP_LOG(INFO) << LOG_DESC("onReceiveTopicSeqMessage") << LOG_KV("nodeID", _nodeID->hex())
                       << LOG_KV("id", _id) << LOG_KV("topicSeq", topicSeq);

        auto buffer = buildEncodedMessage(AMOPMessageType::RequestTopic, bytesConstRef());
        m_frontServiceInterface->asyncSendMessageByNodeID(bcos::protocol::ModuleID::AMOP, _nodeID,
            bytesConstRef(buffer->data(), buffer->size()), 0,
            [_nodeID](Error::Ptr _error, bcos::crypto::NodeIDPtr, bytesConstRef,
                const std::string& _id, bcos::front::ResponseFunc) {
                if (_error && (_error->errorCode() != bcos::protocol::CommonError::SUCCESS))
                {
                    AMOP_LOG(WARNING) << LOG_DESC("onReceiveTopicSeqMessage response")
                                      << LOG_KV("nodeID", _nodeID->hex()) << LOG_KV("id", _id)
                                      << LOG_KV("errorCode", _error->errorCode())
                                      << LOG_KV("errorMessage", _error->errorMessage());
                }
            });
    }
    catch (const std::exception& e)
    {
        AMOP_LOG(ERROR) << LOG_DESC("onReceiveTopicSeqMessage") << LOG_KV("nodeID", _nodeID->hex())
                        << LOG_KV("error", boost::diagnostic_information(e));
    }
}

/**
 * @brief: receive request topic message from other nodes
 * @param _nodeID: the sender nodeID
 * @param _id: the message id
 * @param _data: message data
 * @return void
 */
void AMOP::onReceiveRequestTopicMessage(
    bcos::crypto::NodeIDPtr _nodeID, const std::string& _id, bcos::bytesConstRef _data)
{
    (void)_data;
    try
    {
        std::string topicJson = m_topicManager->queryTopicsSubByClient();

        AMOP_LOG(INFO) << LOG_DESC("onReceiveRequestTopicMessage")
                       << LOG_KV("nodeID", _nodeID->hex()) << LOG_KV("id", _id)
                       << LOG_KV("topicJson", topicJson);

        auto buffer = buildEncodedMessage(AMOPMessageType::ResponseTopic,
            bytesConstRef((byte*)topicJson.data(), topicJson.size()));
        m_frontServiceInterface->asyncSendMessageByNodeID(bcos::protocol::ModuleID::AMOP, _nodeID,
            bytesConstRef(buffer->data(), buffer->size()), 0,
            [_nodeID](Error::Ptr _error, bcos::crypto::NodeIDPtr, bytesConstRef,
                const std::string& _id, bcos::front::ResponseFunc) {
                if (_error && (_error->errorCode() != bcos::protocol::CommonError::SUCCESS))
                {
                    AMOP_LOG(WARNING) << LOG_DESC("onReceiveRequestTopicMessage response")
                                      << LOG_KV("nodeID", _nodeID->hex()) << LOG_KV("id", _id)
                                      << LOG_KV("errorCode", _error->errorCode())
                                      << LOG_KV("errorMessage", _error->errorMessage());
                }
            });
    }
    catch (const std::exception& e)
    {
        AMOP_LOG(ERROR) << LOG_DESC("onReceiveRequestTopicMessage")
                        << LOG_KV("nodeID", _nodeID->hex())
                        << LOG_KV("error", boost::diagnostic_information(e));
    }
}

/**
 * @brief: receive topic response message from other nodes
 * @param _nodeID: the sender nodeID
 * @param _id: the message id
 * @param _data: message data
 * @return void
 */
void AMOP::onReceiveResponseTopicMessage(
    bcos::crypto::NodeIDPtr _nodeID, const std::string& _id, bcos::bytesConstRef _data)
{
    try
    {
        uint32_t topicSeq;
        TopicItems topicItems;
        std::string topicJson = std::string(_data.begin(), _data.end());
        if (m_topicManager->parseTopicItemsJson(topicSeq, topicItems, topicJson))
        {
            m_topicManager->updateSeqAndTopicsByNodeID(_nodeID->hex(), topicSeq, topicItems);
        }
    }
    catch (const std::exception& e)
    {
        AMOP_LOG(ERROR) << LOG_DESC("onReceiveResponseTopicMessage")
                        << LOG_KV("nodeID", _nodeID->hex()) << LOG_KV("id", _id)
                        << LOG_KV("error", boost::diagnostic_information(e));
    }
}

/**
 * @brief: async receive message from front service
 * @param _nodeID: the message sender nodeID
 * @param _id: the id of this message, it can by used to send response to the peer
 * @param _data: the message data
 * @return void
 */
void AMOP::asyncNotifyAmopMessage(
    bcos::crypto::NodeIDPtr _nodeID, const std::string& _id, bcos::bytesConstRef _data)
{
    auto message = m_messageFactory->buildMessage();
    auto size = message->decode(_data);
    if (size < 0)
    {  // invalid format packet
        AMOP_LOG(ERROR) << LOG_DESC("asyncNotifyAmopMessage") << LOG_KV("nodeID", _nodeID->hex())
                        << LOG_KV("id", _id) << LOG_KV("data", *toHexString(_data));
        return;
    }

    auto it = m_msgTypeToHandler.find(message->type());
    if (it != m_msgTypeToHandler.end())
    {
        it->second(_nodeID, _id, _data);
    }
    else
    {
        AMOP_LOG(ERROR) << LOG_DESC("asyncNotifyAmopMessage unrecognized message type")
                        << LOG_KV("type", message->type()) << LOG_KV("nodeID", _nodeID->hex())
                        << LOG_KV("id", _id) << LOG_KV("data", *toHexString(_data));
    }
}

/**
 * @brief: async receive nodeIDs from front service
 * @param _nodeIDs: the nodeIDs
 * @param _callback: callback
 * @return void
 */
void AMOP::asyncNotifyAmopNodeIDs(std::shared_ptr<const crypto::NodeIDs> _nodeIDs,
    std::function<void(bcos::Error::Ptr _error)> _callback)
{
    std::vector<std::string> hexNodeIDs;
    std::for_each(_nodeIDs->begin(), _nodeIDs->end(),
        [&hexNodeIDs](crypto::NodeIDPtr _nodeID) { hexNodeIDs.push_back(_nodeID->hex()); });

    m_topicManager->updateOnlineNodeIDs(hexNodeIDs);
    if (_callback)
    {
        _callback(nullptr);
    }
    AMOP_LOG(INFO) << LOG_DESC("onReceiveResponseTopicMessage")
                   << LOG_KV("nodeIDs size", _nodeIDs->size());
}