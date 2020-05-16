#include "timers.h"

CTimer::CTimer() 
{
    timeStart = std::chrono::steady_clock::now();
}

double CTimer::ElapsedTime() 
{
    timeCurrent = std::chrono::steady_clock::now();
    std::chrono::duration<double> delta = 
        std::chrono::duration_cast<std::chrono::duration<double>>(timeCurrent - timeStart);
    return delta.count();
}

void CTimer::Reset() 
{
    timeStart = timeCurrent;
}
