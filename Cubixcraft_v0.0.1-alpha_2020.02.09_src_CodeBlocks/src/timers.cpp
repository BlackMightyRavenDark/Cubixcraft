#include "timers.h"

Timer::Timer(){
    clock_gettime(CLOCK_REALTIME, &timeStart);
    fpsNano = 0.0;
    passedNs = 0;
}

double Timer::ElapsedTime(){
    clock_gettime(CLOCK_REALTIME, &timeCurrent);
    double delta = (timeCurrent.tv_sec - timeStart.tv_sec) +
        (timeCurrent.tv_nsec - timeStart.tv_nsec) / (double)nsPerSecond;
    passedNs = timeCurrent.tv_nsec - timeStart.tv_nsec;
    if (passedNs < 0)
        passedNs = 0;
    if (passedNs){
        fpsNano = (double)nsPerSecond / (double)passedNs;
    }
    return delta;

}

void Timer::Reset(){
    timeStart = timeCurrent;
}



GameTimer::GameTimer(float tps){
    timeScale = 1.0;
    fps = 0.0;
    passedTime = 0.0;
    ticksPerSecond = tps;
    ticks = 0;
    clock_gettime(CLOCK_REALTIME, &timeLast);
}

void GameTimer::AdvanceTime(){
    timespec timeCurrent;
    clock_gettime(CLOCK_REALTIME, &timeCurrent);
    long passedNs = timeCurrent.tv_nsec - timeLast.tv_nsec;
    timeLast = timeCurrent;
    if (passedNs < 0){
        passedNs = 0;
    }
    if (passedNs > maxNsPerUpdate){
        passedNs = maxNsPerUpdate;
    }
    if (passedNs){
        fps = (float)(nsPerSecond / passedNs);
    }
    passedTime += passedNs * timeScale * ticksPerSecond / nsPerSecond;
    ticks = (int)passedTime;
    if (ticks > maxTicksPerUpdate){
        ticks = maxTicksPerUpdate;
    }
    passedTime -= ticks;
}

