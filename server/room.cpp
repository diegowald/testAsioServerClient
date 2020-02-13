#include "room.h"
#include <numeric>

room::room() : _sumOfValues(0.0), _numElements(0)
{
}

void room::join(participant_ptr participant)
{
    participants_.insert(participant);
    serverMessage sm(serverMessage::MessageType::GlobalAverage);
    sm.setAverage(totalAverage());
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
    double sum = std::accumulate(elements.begin(), elements.end(), 0.0);

    add(sum, elements.size());
    return sum / elements.size();
}

void room::add(double sumOfValues, std::size_t numElements)
{
    {
        std::lock_guard<std::mutex> lock(_calculationMutex);
        _sumOfValues += sumOfValues;
        _numElements += numElements;
    }

    for (auto participant: participants_)
    {
        serverMessage sm(serverMessage::MessageType::GlobalAverage);
        double avg = _sumOfValues / _numElements;
        sm.setAverage(avg);
        participant->deliver(sm);
    }
}

double room::totalAverage() const
{
    return (_numElements == 0) ? 0.0 : _sumOfValues / _numElements;
}

std::size_t room::numElements() const
{
    return _numElements;
}
