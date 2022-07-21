#include "xhtimer.h"
#include "xh_utils.h"

XHTimer::XHTimer(int secs, QObject *parent) :
    QTimer(parent),
    duration(secs),
    counter(0),
    currentState(TimerState::TimerStopped),
    elapsed(0) {

    connect(this, &XHTimer::timeout, this, &XHTimer::updateTimer);

    remainingSeconds = std::chrono::seconds(0);
    elapsedSeconds = std::chrono::seconds(0);
}

int XHTimer::getDuration(){

    return duration;
}

TimerState XHTimer::getTimerState(){

    return currentState;
}

void XHTimer::pause(){

    QTimer::stop();
    currentState = TimerState::TimerPaused;
}

void XHTimer::resume(){

    this->start(XHT_DEFAULT_INTERVALL);
    currentState = TimerState::TimerRunning;
}

void XHTimer::start(int msec){

    if(currentState == TimerState::TimerStopped){
        resetTimer();
    }

    QTimer::start(msec);
    currentState = TimerState::TimerRunning;
}

void XHTimer::stop(){

    QTimer::stop();
    currentState = TimerState::TimerStopped;

	resetTimer();
}

void XHTimer::updateTimer(){

    counter++;

    int temp(0);
    XHDuration t(temp);

    if(counter == duration ){
        currentState = TimerState::TimerJustExpired;
    }

    if(counter <= duration){
        remainingSeconds--;
    }else{
        elapsedSeconds++;
    }

    emit TimerChanged();

    if(currentState == TimerState::TimerJustExpired){
        currentState = TimerState::TimerExpired;
    }
}

XHDuration XHTimer::getElapsed(){

    elapsed = XHDuration(elapsedSeconds.count());

    return elapsed;
}

XHDuration XHTimer::getRemainingTime(){

    remaining = XHDuration(remainingSeconds.count());

    return remaining;
}

void XHTimer::setDuration(int d){

    int h(0), m(0), s(0);

    convertSecsToHMS(d, h, m, s);
    remainingSeconds = std::chrono::seconds(d);
    duration = d;
}

void XHTimer::resetTimer(){

	counter = 0;
	remainingSeconds = std::chrono::seconds(duration);
	elapsedSeconds = std::chrono::seconds(0);
}