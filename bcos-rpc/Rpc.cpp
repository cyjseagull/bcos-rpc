/**
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
 * @brief interface for RPC
 * @file Rpc.cpp
 * @author: octopus
 * @date 2021-07-15
 */

#include <bcos-framework/libutilities/Log.h>
#include <bcos-rpc/Rpc.h>
using namespace bcos;
using namespace bcos::rpc;

void Rpc::start()
{
    // start amop
    m_AMOP->start();
    // start websocket service
    m_wsService->start();
    // start jsonhttp service
    m_httpServer->startListen();
    // start thread for io
    startThread();
    BCOS_LOG(INFO) << LOG_DESC("[RPC][RPC][start]") << LOG_DESC("start amop successfully");
}

void Rpc::stop()
{
    // stop io
    if (m_ioc && !m_ioc->stopped())
    {
        m_ioc->stop();
    }

    // stop thread
    if (m_threads && !m_threads->empty())
    {
        for (auto& t : *m_threads)
        {
            if (t.joinable())
            {
                t.join();
            }
        }
        m_threads->clear();
    }

    // stop http server
    if (m_httpServer)
    {
        m_httpServer->stop();
    }

    // stop ws service
    if (m_wsService)
    {
        m_wsService->stop();
    }

    // stop amop
    if (m_AMOP)
    {
        m_AMOP->stop();
    }

    BCOS_LOG(INFO) << LOG_DESC("[RPC][RPC][stop]") << LOG_DESC("stop amop successfully");
}

/**
 * @brief: notify blockNumber to rpc
 * @param _blockNumber: blockNumber
 * @param _callback: resp callback
 * @return void
 */
void Rpc::asyncNotifyBlockNumber(
    bcos::protocol::BlockNumber _blockNumber, std::function<void(Error::Ptr)> _callback)
{
    m_wsService->notifyBlockNumberToClient(_blockNumber);
    if (_callback)
    {
        _callback(nullptr);
    }
}

/**
 * @brief: async receive message from front service
 * @param _nodeID: the message sender nodeID
 * @param _id: the id of this message, it can by used to send response to the peer
 * @param _data: the message data
 * @return void
 */
void Rpc::asyncNotifyAmopMessage(bcos::crypto::NodeIDPtr _nodeID, const std::string& _id,
    bcos::bytesConstRef _data, std::function<void(Error::Ptr _error)> _onRecv)
{
    m_AMOP->asyncNotifyAmopMessage(_nodeID, _id, _data, _onRecv);
}

/**
 * @brief: async receive nodeIDs from front service
 * @param _nodeIDs: the nodeIDs
 * @param _callback: callback
 * @return void
 */
void Rpc::asyncNotifyAmopNodeIDs(std::shared_ptr<const bcos::crypto::NodeIDs> _nodeIDs,
    std::function<void(bcos::Error::Ptr _error)> _callback)
{
    m_AMOP->asyncNotifyAmopNodeIDs(_nodeIDs, _callback);
}