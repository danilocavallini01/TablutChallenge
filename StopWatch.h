#include <chrono>

class StopWatch
{
private:
    std::chrono::high_resolution_clock::time_point _startPoint;
    int _timeLimit;
    bool _timeout;

public:
    StopWatch(const int __timeLimit = 59500);
    ~StopWatch();

    bool isTimeouted();

    void reset();

    void start();
    void setTimeLimit(const int __timeLimit);

    int getTimeLimit();
};