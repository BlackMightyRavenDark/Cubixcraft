#include "timers.h"

Timer::Timer() {
    timeStart = std::chrono::steady_clock::now();
}

double Timer::ElapsedTime() {
    timeCurrent = std::chrono::steady_clock::now();
    std::chrono::duration<double> delta = 
        std::chrono::duration_cast<std::chrono::duration<double>>(timeCurrent - timeStart);
    return delta.count();
}

void Timer::Reset() {
    timeStart = timeCurrent;
}
