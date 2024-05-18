#include <chrono>

class StopWatch
{
private:
    std::chrono::high_resolution_clock::time_point _startPoint;
    int _timeLimit;
    bool _timeout;

public:
    StopWatch(const int __timeLimit = false) : _timeout(false)
    {
        _timeLimit = __timeLimit;
    }

    ~StopWatch() {}

    bool isTimeouted()
    {
        if (_timeout)
        {
            return true;
        }

        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - _startPoint).count() > _timeLimit)
        {
            _timeout = true;
        }

        return _timeout;
    }

    void reset()
    {
        _timeout = false;
    }

    void start()
    {
        _startPoint = std::chrono::high_resolution_clock::now();
    }

    int getRemainingTime() {
        return _timeout ? 0 : _timeLimit - std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - _startPoint).count();
    }

    void setTimeLimit(const int __timeLimit)
    {
        _timeLimit = __timeLimit;
    }

    int getTimeLimit()
    {
        return _timeLimit;
    }
};