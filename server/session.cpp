#include "session.h"
#include <memory>
#include <numeric>

session::session(asio::ip::tcp::socket socket, room& room)
    : socket_(std::move(socket)),
      room_(room)
{
}

void session::start(std::size_t maxNumElements)
{
    room_.join(shared_from_this());
    serverMessage sm(serverMessage::MessageType::MaxNumElements);
    sm.setMaxNumElements(maxNumElements);
    _maxNumElements = maxNumElements;
    deliver(sm);
    do_read_header();
}

void session::deliver(const serverMessage &msg)
{
    bool write_in_progress = !_write_msgs.empty();
    _write_msgs.push_back(msg);
    if (!write_in_progress)
    {
        do_write();
    }
}


void session::do_read_header()
{
    auto self(shared_from_this());
    asio::async_read(socket_,
                     asio::buffer(_read_msg.header(), sizeof(std::size_t)),
                     [this, self](std::error_code ec, std::size_t /*length*/)
    {
        if (!ec && _read_msg.validateHeader(_maxNumElements))
        {
            do_read_body();
        }
        else
        {
            room_.leave(shared_from_this());
        }
    });
}

double session::sum(std::vector<double> &elements)
{
    return std::accumulate(elements.begin(), elements.end(), 0.0);
}

void session::do_read_body()
{
    auto self(shared_from_this());
    asio::async_read(socket_,
                     asio::buffer(_read_msg.vector(), _read_msg.vectorSizeBytes()),
                     [this, self](std::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {
            std::vector<double> elements = _read_msg.elements();
            double total = sum(elements);
            double avg = total / elements.size();
            serverMessage sm(serverMessage::MessageType::VectorAverage);
            sm.setAverage(avg);
            deliver(sm);
            room_.add(total, elements.size());
            do_read_header();
        }
        else
        {
            room_.leave(shared_from_this());
        }
    });
}

void session::do_write()
{
    auto self(shared_from_this());
    asio::async_write(socket_,
                      asio::buffer(_write_msgs.front().data(),
                                   _write_msgs.front().length()),
                      [this, self](std::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {
            _write_msgs.pop_front();
            if (!_write_msgs.empty())
            {
                do_write();
            }
        }
        else
        {
            room_.leave(shared_from_this());
        }
    });
}
