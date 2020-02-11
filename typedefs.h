#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <deque>
#include <memory>

//class message;
class clientMessage;
class serverMessage;

//typedef std::deque<message> message_queue;
typedef std::deque<serverMessage> serverMessage_queue;

class participant
{
public:
    virtual ~participant() {}
    //virtual void deliver(const message& msg) = 0;
    virtual void deliver(const serverMessage &msg) = 0;
};

typedef std::shared_ptr<participant> participant_ptr;

class room;

#endif // TYPEDEFS_H
