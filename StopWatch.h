#include <chrono>
#include <atomic>

class StopWatch
{
    std::chrono::high_resolution_clock::time_point _startPoint;
    int _timeLimit;

public:
    StopWatch(const int __timeLimit) : _startPoint(std::chrono::high_resolution_clock::now())
    {
        _timeLimit = __timeLimit;
    }

    inline bool timeout() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(_startPoint - std::chrono::high_resolution_clock::now()).count() > _timeLimit;
    }
};