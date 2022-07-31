#ifndef XH_UTILS_H
#define XH_UTILS_H

#include <string>

#include <QMargins>

void convertSecsToHMS(const int totalSecs, int &h, int &m, int &s);
int convertToInt(const std::string& str);

QMargins createMargins(int val);

#endif // XH_UTILS_H
