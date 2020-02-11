#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <asio.hpp>

#include "../typedefs.h"
#include "room.h"

class server
{
public:
  server(std::size_t maxVectorSize, asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint);

private:
  void do_accept();

  asio::ip::tcp::acceptor acceptor_;
  room room_;

  std::size_t _maxVectorSize;
};

#endif // SERVER_H
