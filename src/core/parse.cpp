#include <iostream>

#include "parse.h"
#include "xh_utils.h"

#include <QRegularExpression>

void process(std::string &exp, int &h, int &m, int &s){
	h = m = s = 0;

	// Inspired by "TimeSpanTokenShortFormPattern" and "TimeSpanTokenLongFormPattern" from Chris Dziemborowicz
	// https://github.com/dziemborowicz/hourglass/blob/21589c76c9d6e21bf4b523b90c9bd56b8889f36c/Hourglass/Properties/Resources.resx#L300-L412
	const QStringList patterns = {
		"^((?<hours>\\d+):)?(?<minutes>\\d+):(?<seconds>\\d+)$", // 45:36, 12:45:36
		"^(?<minutes>\\d+)$", // 45 -> 45m
		"^(?<minutes>\\d+)m(?<seconds>\\d+)$", // 45m36
		"^(?<hours>\\d+)h(?<minutes>\\d+)?$", // 12h[45]
		"^(?<hours>\\d+)h(?<minutes>\\d+)m(?<seconds>\\d+)$", // 12h45m36
		"^((?<hours>\\d+)h)?((?<minutes>\\d+)m)?((?<seconds>\\d+)s)?$" // [12h][45m][36s]
	};

	QString durationText(exp.c_str());

	for (int i = 0; i < patterns.size(); ++i) {

		QString pattern(patterns.at(i));
		QRegularExpression regex(pattern);

		QRegularExpressionMatch match = regex.match(durationText);

		if (match.hasMatch()) {

			if (!match.captured("hours").isNull()) {
				h = match.captured("hours").toInt();
			}

			if (!match.captured("minutes").isNull()) {
				m = match.captured("minutes").toInt();
			}

			if (!match.captured("seconds").isNull()) {
				s = match.captured("seconds").toInt();
			}

			break;
		}
	}
}

int processInput(std::string &input){

	int secs(0), mins(0), hours(0), totalSecs(0);

	// remove whitespace
	for (std::string::iterator it = input.begin(); it != input.end(); ++it) {
		if (isspace(*it)) {
			std::string::iterator nextIt = input.erase(it);
			it = nextIt-1;
		}
	}

	if (input.empty()) {
		return 0;
	}

	process(input, hours, mins, secs);
	totalSecs = hours * 3600 + mins * 60 + secs;

	return totalSecs;
}
