#include "xhduration.h"
#include "xh_utils.h"

XHDuration::XHDuration(int secs) :
    formatted(tr("")), hours(0), minutes(0), seconds(0), totalInSeconds(secs){

    formatTime(secs);
}

int XHDuration::getHours(){
    return hours;
}

int XHDuration::getMinutes(){
    return minutes;
}

int XHDuration::getSeconds(){
    return seconds;
}

QString XHDuration::getFormatted(){
    return formatted;
}

int XHDuration::getTotalSecs(){
    return totalInSeconds;
}

void XHDuration::formatTime(int durationInSeconds){

	convertSecsToHMS(durationInSeconds, hours, minutes, seconds);
	
    if(hours != 0){
        formatted = QString(tr("%1 hours %2 minutes %3 seconds"))
                    .arg(hours)
                    .arg(minutes)
                    .arg(seconds);
    }else if(minutes != 0){
        formatted = QString(tr("%1 minutes %2 seconds"))
                    .arg(minutes)
                    .arg(seconds);
    }else{
        formatted = QString(tr("%1 seconds")).arg(seconds);
    }
}
