#include "client.h"

#include <random>

client::client(asio::io_service& io_service, asio::ip::tcp::resolver::iterator endpoint_iterator)
    : io_service_(io_service), socket_(io_service), _read_msg(serverMessage::MessageType::UnknownMessage)
{
    _timer = nullptr;

    do_connect(endpoint_iterator);
}

void client::write(const clientMessage& msg)
{
    io_service_.post(
                [this, msg]()
    {
        bool write_in_progress = !_write_msgs.empty();
        _write_msgs.push_back(msg);
        if (!write_in_progress)
        {
            do_write();
        }
    });
}

void client::close()
{
    io_service_.post([this]() { socket_.close(); });
}

void client::do_connect(asio::ip::tcp::resolver::iterator endpoint_iterator)
{
    asio::async_connect(socket_, endpoint_iterator,
                        [this](std::error_code ec, asio::ip::tcp::resolver::iterator)
    {
        if (!ec)
        {
            do_read_header();
        }
    });
}

void client::do_read_header()
{
    asio::async_read(socket_,
                     asio::buffer(_read_msg.header(), sizeof(int)),
                     [this](std::error_code ec, std::size_t /*length*/)
    {
        if (!ec && _read_msg.decode_header() && _read_msg.messageType() != serverMessage::MessageType::UnknownMessage)
        {
            do_read_body();
        }
        else
        {
            socket_.close();
        }
    });
}

void client::do_read_body()
{
    asio::async_read(socket_,
                     asio::buffer(_read_msg.body(), _read_msg.body_lengthBytes()),
                     [this](std::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {
            switch (_read_msg.messageType())
            {
            case serverMessage::MessageType::MaxNumElements:
                  _maxElementCount = _read_msg.maxNumElements();
                  std::cout << "Max Number of elements: " << _maxElementCount << std::endl;
                  startPeriodicTimer();
                break;
            case serverMessage::MessageType::GlobalAverage:
                std::cout << "Gloal Average: " << _read_msg.average() << std::endl;
                break;
            case serverMessage::MessageType::VectorAverage:
                std::cout << "Vector Average: " << _read_msg.average() << std::endl;
                break;
            }
            do_read_header();
        }
        else
        {
            socket_.close();
        }
    });
}

void client::do_write()
{
    asio::async_write(socket_,
                      asio::buffer(_write_msgs.front().data(),
                                   _write_msgs.front().dataLengthBytes()),
                      [this](std::error_code ec, std::size_t /*length*/)
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
            socket_.close();
        }
    });
}

void client::startPeriodicTimer()
{
    if (_timer == nullptr)
    {
        _timer = new asio::steady_timer(io_service_, asio::chrono::seconds(5));
        _timer->async_wait([this](const std::error_code &ec)
        {
            if (!ec)
            {
                prepareVector();
                sendVector();
            }
        });
    }
}

void client::prepareVector()
{
    _elements.clear();
    std::default_random_engine re;

    for (std::size_t i = 0; i < _maxElementCount; ++i)
    {
        _elements[i] = _unif(re);
    }
}


void client::sendVector()
{
    clientMessage cm;
    cm.setElements(_elements);
    write(cm);
}
