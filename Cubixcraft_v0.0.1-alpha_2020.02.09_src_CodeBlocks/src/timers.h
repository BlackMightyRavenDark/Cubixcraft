#include <ctime>

class Timer {

private:
    timespec timeStart;
    timespec timeCurrent;
    const long nsPerSecond = 1000000000;

public:
    double fpsNano;
    long passedNs;

    Timer();
    double ElapsedTime();
    void Reset();
};


class GameTimer {
private:
    timespec timeLast;
    const long nsPerSecond    = 1000000000;
    const long maxNsPerUpdate = 1000000000;
    const int maxTicksPerUpdate = 100;
    float ticksPerSecond;
    float timeScale;
    float fps;
    float passedTime;

public:
    int ticks;

    GameTimer(float tps);
    void AdvanceTime();
};
