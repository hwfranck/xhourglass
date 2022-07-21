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

	QString hoursString(tr("%n hour(s) ", "", hours)),
		minutesString(tr("%n minute(s) ", "", minutes)),
		secondsString(tr("%n second(s)", "", seconds));

    if(hours != 0){
        formatted = hoursString
					.append(minutesString)
					.append(secondsString);
    }else if(minutes != 0){
        formatted = minutesString
					.append(secondsString);
    }else{
        formatted = secondsString;
    }
}
