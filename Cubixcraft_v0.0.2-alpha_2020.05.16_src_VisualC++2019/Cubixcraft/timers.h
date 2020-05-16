#include <chrono>

class CTimer {
public:
    CTimer();
    double ElapsedTime();
    void Reset();

private:
    std::chrono::steady_clock::time_point timeStart;
    std::chrono::steady_clock::time_point timeCurrent;
    const long nsPerSecond = 1000000000;
};
