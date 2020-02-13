#ifndef ROOM_H
#define ROOM_H

#include <set>
#include <vector>
#include <mutex>

#include "../typedefs.h"
#include "../message.h"

class room
{
public:
    room();
    void join(participant_ptr participant);
    void leave(participant_ptr participant);
    //void deliver(const message& msg);
    double deliver(const std::vector<double>& elements);

    void add(double sumOfValues, std::size_t numElements);
    double totalAverage() const;
    std::size_t numElements() const;

private:
    std::set<participant_ptr> participants_;

    double _sumOfValues;
    std::size_t _numElements;
    std::mutex _calculationMutex;
};

#endif // ROOM_H
