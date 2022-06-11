#include <iostream>

#include "parse.h"
#include "xh_utils.h"

void process(std::string &exp, int &h, int &m, int &s){
        h = m = s = 0;
        size_t hPos, mPos, sPos;

        bool validExpression(true);
        size_t indexIntruder = exp.find_first_not_of("0123456789ms:");

        if( indexIntruder != std::string::npos){
            validExpression = false;
            //	std::cout << "\n 1<" << exp[indexIntruder] << ">";
            return;
        }

		// Parse colon-separated durations. Can only contain 1 or 2 colons.
        if( exp.find(":") != std::string::npos ){

            int numberOfColons(0);
            size_t i(0);

			// Keep searching on the unexplored substring
            i = exp.find(":");
            while( i != std::string::npos){
                numberOfColons++;
                i = exp.find(":", i+1);
            }

            if(numberOfColons == 1){
                mPos = exp.find(":");

                h = convertToInt( exp );
                m = convertToInt ( exp.substr(mPos+1) );
            }else if(numberOfColons == 2){
                hPos = exp.find(":");
                mPos = exp.rfind(":");

                h = convertToInt( exp );
                m = convertToInt( exp.substr(hPos+1, mPos-1) );
                s = convertToInt( exp.substr(mPos+1));
            }

            return;
        }

        mPos = exp.find("m");
        sPos = exp.find("s");

        if( exp.find("m") != mPos || exp.find("s") != sPos ){
            validExpression = false;
        }

        if(validExpression){

            if(mPos != std::string::npos && sPos != std::string::npos ){
                m = convertToInt( exp );
                s = convertToInt( exp.substr(mPos+1, sPos-mPos-1) );
            }else if(mPos != std::string::npos ){
                m = std::stoi(exp);

                s = convertToInt(exp.substr(mPos+1));
            }else if(sPos != std::string::npos ){
                s = convertToInt(exp);
            }else{
                m = convertToInt(exp);
            }
        }else{
            //	std::cout << "\n!! 2<" << exp << "> !!";
        }
}

int processInput(std::string &input){

    int secs(0), mins(0), hours(0), totalSecs(0);

    std::string durationText(input);

    process(durationText, hours, mins, secs);
    totalSecs = hours * 3600 + mins * 60 + secs;

    return totalSecs;
}
