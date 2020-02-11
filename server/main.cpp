//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include "asio.hpp"
#include "../message.h"

#include "server.h"


using asio::ip::tcp;




//----------------------------------------------------------------------



//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 3)
    {
      std::cerr << "Usage: server maxVectorSize <port> [<port> ...]\n";
      return 1;
    }

    asio::io_context io_context;

    std::list<server> servers;

    std::size_t maxVectorSize = std::atoi(argv[1]);

    for (int i = 2; i < argc; ++i)
    {
      tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
      servers.emplace_back(maxVectorSize, io_context, endpoint);
    }

    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
