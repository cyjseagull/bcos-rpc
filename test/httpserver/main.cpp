//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#include <bcos-rpc/rpc/RpcFactory.h>
#include <iostream>

//------------------------------------------------------------------------------
// Accepts incoming connections and launches the Sessions
//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    // Check command line arguments.
    if (argc != 3)
    {
        std::cerr << "Usage: ./http-server <address> <port>\n"
                  << "Example:\n"
                  << "    ./http-server 0.0.0.0 8080\n";
        return -1;
    }

    auto const address = argv[1];
    auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
    auto const threads = std::max<int>(1, std::thread::hardware_concurrency());

    auto factory = std::make_shared<bcos::rpc::RpcFactory>();

    bcos::rpc::RpcConfig config;
    config.m_threadCount = threads;
    config.m_listenIP = address;
    config.m_listenPort = port;

    auto rpc = factory->buildRpc(config);
    rpc->start();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100000));
    }

    return 0;
}