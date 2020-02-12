#ifndef SESSION_H
#define SESSION_H

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <asio.hpp>

#include "../message.h"

#include "../typedefs.h"
#include "room.h"
//----------------------------------------------------------------------

class session : public participant,
    public std::enable_shared_from_this<session>
{
public:
  session(asio::ip::tcp::socket socket, room& room);

  void start(std::size_t maxNumElements);
  virtual void deliver(const serverMessage &msg) override;

private:
  void do_read_header();
  void do_read_body();
  void do_writeHeader();
  void do_writePayload();
  double sum(std::vector<double> &elements);

  asio::ip::tcp::socket socket_;
  room& room_;
  clientMessage _read_msg;
  //message read_msg_;
  //message_queue write_msgs_;
  serverMessage_queue _write_msgs;
  std::size_t _maxNumElements;
};


#endif // SESSION_H
