#ifndef XHTOASTHANDLER_H
#define XHTOASTHANDLER_H

#ifdef Q_OS_WIN
	#include "xhtoasthandler_win.h"
#else
	// placeholder to prevent crashes on unsupported platforms
	class XHToastHandler {

		public:
			XHToastHandler(){}

			void show() {}
	};
#endif

#endif // XHTOASTHANDLER_H
