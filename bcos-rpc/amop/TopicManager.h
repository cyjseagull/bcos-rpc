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
 * @file TopicManager.h
 * @author: octopus
 * @date 2021-06-18
 */
#pragma once
#include <bcos-framework/interfaces/crypto/KeyInterface.h>
#include <bcos-framework/libutilities/Common.h>
#include <bcos-rpc/amop/Common.h>
#include <shared_mutex>

namespace bcos
{
namespace amop
{
class TopicManager : public std::enable_shared_from_this<TopicManager>
{
public:
    using Ptr = std::shared_ptr<TopicManager>;

private:
    // m_clientToTopicItems lock
    mutable std::shared_mutex x_clientTopics;
    // topicSeq
    std::atomic<uint32_t> m_topicSeq{1};
    // clientID => TopicItems
    std::unordered_map<std::string, TopicItems> m_clientToTopicItems;

    // m_nodeIDToTopicSeq lock
    mutable std::shared_mutex x_topics;
    // nodeID => topicSeq
    std::unordered_map<bcos::crypto::NodeIDPtr, uint32_t> m_nodeIDToTopicSeq;
    // nodeID => topicItems
    std::unordered_map<bcos::crypto::NodeIDPtr, TopicItems> m_nodeIDToTopicItems;

public:
    uint32_t topicSeq() const { return m_topicSeq; }
    uint32_t incTopicSeq()
    {
        uint32_t topicSeq = ++m_topicSeq;
        return topicSeq;
    }

public:
    /**
     * @brief: client subscribe topic
     * @param _clientID: client identify, to be defined
     * @param _topicItems: topics subscribe by client
     * @return void
     */
    void clientSubTopic(const std::string& _clientID, const TopicItems& _topicItems);
    /**
     * @brief: query topics sub by client
     * @param _clientID: client identify, to be defined
     * @param _topicItems: topics subscribe by client
     * @return bool
     */
    bool queryTopicItemsByClient(const std::string& _clientID, TopicItems& _topicItems);
    /**
     * @brief: remove all topics subscribed by client
     * @param _clientID: client identify, to be defined
     * @return void
     */
    void removeTopicsByClient(const std::string& _clientID);
    /**
     * @brief: query topics subscribed by all connected clients
     * @return json string result, include topicSeq and topicItems fields
     */
    std::string queryTopicsSubByClient();
    /**
     * @brief: parse json to fetch topicSeq and topicItems
     * @param _topicSeq: return value, topicSeq
     * @param _topicItems: return value, topics
     * @param _json: json
     * @return void
     */
    bool parseTopicItemsJson(
        uint32_t& _topicSeq, TopicItems& _topicItems, const std::string& _json);
    /**
     * @brief: check if the topicSeq of nodeID changed
     * @param _nodeID: the peer nodeID
     * @param _topicSeq: the topicSeq of the nodeID
     * @return bool: if the nodeID has been changed
     */
    bool checkTopicSeq(bcos::crypto::NodeIDPtr _nodeID, uint32_t _topicSeq);
    /**
     * @brief: update online nodeIDs, clean up the offline nodeIDs state
     * @param _nodeIDs: the online nodeIDs
     * @return void
     */
    void updateOnlineNodeIDs(const bcos::crypto::NodeIDs& _nodeIDs);
    /**
     * @brief: update the topicSeq and topicItems of the nodeID's
     * @param _nodeID: nodeID
     * @param _topicSeq: topicSeq
     * @param _topicItems: topicItems
     * @return void
     */
    void updateSeqAndTopicsByNodeID(
        bcos::crypto::NodeIDPtr _nodeID, uint32_t _topicSeq, const TopicItems& _topicItems);
    /**
     * @brief: find the nodeIDs by topic
     * @param _topic: topic
     * @param _nodeIDs: nodeIDs
     * @return void
     */
    void queryNodeIDsByTopic(const std::string& _topic, bcos::crypto::NodeIDs& _nodeIDs);
};
}  // namespace amop
}  // namespace bcos