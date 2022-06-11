#ifndef XHTIMER_H
#define XHTIMER_H

#define XHT_DEFAULT_INTERVALL 1000

#include <QTimer>

#include <chrono>

#include "xhduration.h"

class QTime;

enum class TimerState{
    TimerStarted,
    TimerRunning,
    TimerPaused,
    TimerExpired,
    TimerJustExpired,
    TimerStopped
};

class XHTimer : public QTimer{

    Q_OBJECT

    public:
        XHTimer(int secs, QObject *parent = nullptr);
        int getDuration();
        void setDuration(int duration);
        TimerState getTimerState();
        int getTotalRemaining();
        XHDuration getElapsed();
        XHDuration getRemainingTime();

        void pause();
        void resume();

    signals :
        void TimerChanged();

    public slots :
        void start(int msec = XHT_DEFAULT_INTERVALL);
        void stop();

    private slots:
        void updateTimer();

    private:
        int duration,
            counter;

        std::chrono::seconds remainingSeconds,
                            elapsedSeconds;
        TimerState currentState;

        XHDuration elapsed,
                remaining;
};

#endif // XHTIMER_H
