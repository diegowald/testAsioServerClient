#ifndef CLIENT_H
#define CLIENT_H

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <asio.hpp>
#include <vector>
#include <random>

#include "../typedefs.h"
#include "../message.h"

typedef std::deque<clientMessage> message_queue;

class client
{
public:
  client(asio::io_service& io_service, asio::ip::tcp::resolver::iterator endpoint_iterator);

  void write(const clientMessage& msg);

  void close();
private:
  void do_connect(asio::ip::tcp::resolver::iterator endpoint_iterator);
  void do_read_header();

  void do_read_body();

  void do_writeHeader();
  void do_writePayload();

  void startPeriodicTimer();
  void prepareVector();
  void sendVector();

private:
  asio::io_service& io_service_;
  asio::ip::tcp::socket socket_;
  serverMessage _read_msg;
  message_queue _write_msgs;
  std::size_t _maxElementCount;
  asio::steady_timer *_timer;
  std::vector<double> _elements;
  std::uniform_real_distribution<double> _unif;
};


#endif // CLIENT_H
