#include "room.h"

room::room() : _sumOfValues(0.0), _numElements(0)
{
}

void room::join(participant_ptr participant)
{
    participants_.insert(participant);
    serverMessage sm(serverMessage::MessageType::GlobalAverage);
    participant->deliver(sm);
}

void room::leave(participant_ptr participant)
{
    participants_.erase(participant);
}

/*void room::deliver(const message& msg)
{
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
        recent_msgs_.pop_front();

    for (auto participant: participants_)
        participant->deliver(msg);
}*/


double room::deliver(const std::vector<double>& elements)
{
    double sum = 0.0;
    for (float element: elements)
    {
        sum += element;
    }

    add(sum, elements.size());
    return sum / elements.size();
}

void room::add(double sumOfValues, std::size_t numElements)
{
    _sumOfValues += sumOfValues;
    _numElements += numElements;

    for (auto participant: participants_)
    {
        serverMessage sm(serverMessage::MessageType::GlobalAverage);
        double avg = _sumOfValues / _numElements;
        sm.setAverage(avg);
        participant->deliver(sm);
    }
}

float room::totalAverage() const
{
    return _sumOfValues / _numElements;
}

std::size_t room::numElements() const
{
    return _numElements;
}
