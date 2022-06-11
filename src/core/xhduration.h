#ifndef XHDURATION_H
#define XHDURATION_H

#include <QString>
#include <QCoreApplication>

class XHDuration{

    Q_DECLARE_TR_FUNCTIONS(XHDuration)

    public:
        XHDuration(int t = 0);

        QString getFormatted();
        int getHours();
        int getMinutes();
        int getSeconds();
        int getTotalSecs();

    private:
        void formatTime(int);

        QString formatted;
        int hours,
            minutes,
            seconds,
            totalInSeconds;
};

#endif // XHDURATION_H
