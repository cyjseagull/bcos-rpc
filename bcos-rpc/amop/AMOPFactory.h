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
 * @file AMOPFactory.h
 * @author: octopus
 * @date 2021-06-21
 */
#pragma once

#include <bcos-framework/interfaces/front/FrontServiceInterface.h>
#include <bcos-rpc/amop/AMOP.h>
#include <memory>

namespace bcos
{
namespace amop
{
class AMOPFactory
{
public:
    using Ptr = std::shared_ptr<AMOPFactory>;

public:
    /**
     * @brief: create AMOP
     * @param _frontServiceInterface: front service interface
     * @return AMOP::Ptr
     */
    AMOP::Ptr buildAmop(bcos::front::FrontServiceInterface::Ptr _frontServiceInterface)
    {
        auto amop = std::make_shared<AMOP>();
        auto messageFactory = std::make_shared<MessageFactory>();
        auto topicManager = std::make_shared<TopicManager>();
        auto ioService = std::make_shared<boost::asio::io_service>();
        amop->setFrontServiceInterface(_frontServiceInterface);
        amop->setMessageFactory(messageFactory);
        amop->setTopicManager(topicManager);
        amop->setIoService(ioService);
        return amop;
    }
};
}  // namespace amop
}  // namespace  bcos
