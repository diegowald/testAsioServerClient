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
            do_writeHeader();
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
        if (!ec && _read_msg.validateHeader() && _read_msg.messageType() != serverMessage::MessageType::UnknownMessage)
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
                     asio::buffer(_read_msg.payload(), _read_msg.payloadSizeBytes()),
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
            case serverMessage::MessageType::UnknownMessage:
                std::cout << "Error";
            }
            do_read_header();
        }
        else
        {
            socket_.close();
        }
    });
}

void client::do_writeHeader()
{
    asio::async_write(socket_,
                      asio::buffer(_write_msgs.front().header(),
                                   _write_msgs.front().headerSizeBytes()),
                      [this](std::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {
            do_writePayload();
        }
        else
        {
            socket_.close();
        }
    });
}

void client::do_writePayload()
{
    asio::async_write(socket_,
                      asio::buffer(_write_msgs.front().payload(),
                                   _write_msgs.front().payloadSizeBytes()),
                      [this](std::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {
            _write_msgs.pop_front();
            if (!_write_msgs.empty())
            {
                do_writeHeader();
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
    _elements.resize(_maxElementCount);
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
