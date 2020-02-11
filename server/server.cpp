#include "server.h"

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>

#include "session.h"

server::server(std::size_t maxVectorSize, asio::io_context& io_context,
               const asio::ip::tcp::endpoint& endpoint) : acceptor_(io_context, endpoint),
    _maxVectorSize(maxVectorSize)
{
    do_accept();
}

void server::do_accept()
{
    acceptor_.async_accept(
                [this](std::error_code ec, asio::ip::tcp::socket socket)
    {
        if (!ec)
        {
            std::cout << "Incomming connection accepted" << std::endl;
            std::make_shared<session>(std::move(socket), room_)->start(_maxVectorSize);
        }

        do_accept();
    });
}
