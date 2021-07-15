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
 * @brief RpcFactory
 * @file RpcFactory.h
 * @author: octopus
 * @date 2021-07-15
 */

#include <bcos-framework/libutilities/Exceptions.h>
#include <bcos-rpc/rpc/RpcFactory.h>
#include <bcos-rpc/rpc/http/HttpServer.h>
#include <bcos-rpc/rpc/jsonrpc/JsonRpcImpl_2_0.h>

using namespace bcos;
using namespace bcos::rpc;
using namespace bcos::http;

void RpcFactory::checkParams()
{
    if (!m_ledgerInterface)
    {
        BOOST_THROW_EXCEPTION(InvalidParameter() << errinfo_comment(
                                  "RpcFactory::checkParams ledgerInterface is uninitialized"));
    }

    if (!m_executorInterface)
    {
        BOOST_THROW_EXCEPTION(InvalidParameter() << errinfo_comment(
                                  "RpcFactory::checkParams executorInterface is uninitialized"));
    }

    if (!m_txPoolInterface)
    {
        BOOST_THROW_EXCEPTION(InvalidParameter() << errinfo_comment(
                                  "RpcFactory::checkParams txPoolInterface is uninitialized"));
    }

    if (!m_consensusInterface)
    {
        BOOST_THROW_EXCEPTION(InvalidParameter() << errinfo_comment(
                                  "RpcFactory::checkParams consensusInterface is uninitialized"));
    }

    if (!m_blockSyncInterface)
    {
        BOOST_THROW_EXCEPTION(InvalidParameter() << errinfo_comment(
                                  "RpcFactory::checkParams blockSyncInterface is uninitialized"));
    }

    if (!m_transactionFactory)
    {
        BOOST_THROW_EXCEPTION(InvalidParameter() << errinfo_comment(
                                  "RpcFactory::checkParams transactionFactory is uninitialized"));
    }
    return;
}

/**
 * @brief:  Rpc
 * @param _listenIP: listen ip
 * @param _listenPort: listen port
 * @param _threadCount: thread count
 * @return Rpc::Ptr:
 */
Rpc::Ptr RpcFactory::buildRpc(
    const std::string& _listenIP, uint16_t _listenPort, std::size_t _threadCount)
{
    // TODO: for test
    // checkParams();

    auto rpc = std::make_shared<Rpc>();
    auto jsonRpcInterface = std::make_shared<bcos::rpc::JsonRpcImpl_2_0>();

    jsonRpcInterface->setLedger(m_ledgerInterface);
    jsonRpcInterface->setTxPoolInterface(m_txPoolInterface);
    jsonRpcInterface->setExecutorInterface(m_executorInterface);
    jsonRpcInterface->setConsensusInterface(m_consensusInterface);
    jsonRpcInterface->setBlockSyncInterface(m_blockSyncInterface);
    jsonRpcInterface->setTransactionFactory(m_transactionFactory);

    auto httpServerFactory = std::make_shared<bcos::http::HttpServerFactory>();
    auto httpServer = httpServerFactory->buildHttpServer(_listenIP, _listenPort, _threadCount);
    httpServer->setRequestHandler(std::bind(&bcos::rpc::JsonRpcImpl_2_0::onRPCRequest,
        jsonRpcInterface, std::placeholders::_1, std::placeholders::_2));

    rpc->setHttpServer(httpServer);
    RPC_FACTORY(INFO) << LOG_DESC("buildRpc") << LOG_KV("listenIP", _listenIP)
                      << LOG_KV("listenPort", _listenPort) << LOG_KV("threadCount", _threadCount);
    return rpc;
}