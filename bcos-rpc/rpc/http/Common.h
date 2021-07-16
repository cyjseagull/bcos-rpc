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
 * @file Common.h
 * @author: octopus
 * @date 2021-07-08
 */
#pragma once

#include <bcos-framework/libutilities/Log.h>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#define HTTP_LISTEN(LEVEL) BCOS_LOG(LEVEL) << "[HTTP][LISTEN]"
#define HTTP_SESSION(LEVEL) BCOS_LOG(LEVEL) << "[HTTP][SESSION]"
#define HTTP_SERVER(LEVEL) BCOS_LOG(LEVEL) << "[HTTP][SERVER]"

namespace bcos
{
namespace http
{
using RequestHandler =
    std::function<void(const std::string& req, std::function<void(const std::string& resp)>)>;
using HttpRequest = boost::beast::http::request<boost::beast::http::string_body>;
using HttpResponse = boost::beast::http::response<boost::beast::http::string_body>;
using HttpResponsePtr = std::shared_ptr<HttpResponse>;
}  // namespace http
};  // namespace bcos