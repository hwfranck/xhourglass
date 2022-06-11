#include "xh_utils.h"

void convertSecsToHMS(const int totalSecs, int &h, int &m, int &s){

    h = (totalSecs / 3600);
    m = (totalSecs % 3600) / 60;
    s = (totalSecs % 60);
}

int convertToInt(std::string &str){

    int result = 0;

    if(!str.empty()){
        result = std::stoi(str);
    }

    return result;
}
