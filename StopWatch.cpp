#include "StopWatch.h"

StopWatch::StopWatch(const int __timeLimit)
{
    _timeLimit = __timeLimit;
    _timeout = false;
}

StopWatch::~StopWatch() {}

bool StopWatch::isTimeouted()
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

void StopWatch::reset()
{
    _timeout = false;
}

void StopWatch::start()
{
    _startPoint = std::chrono::high_resolution_clock::now();
}

void StopWatch::setTimeLimit(const int __timeLimit)
{
    _timeLimit = __timeLimit;
}

int StopWatch::getTimeLimit()
{
    return _timeLimit;
}