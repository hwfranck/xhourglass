#ifndef XHTOASTHANDLER_LINUX_H
#define XHTOASTHANDLER_LINUX_H

#include <QDebug>

class XHToastHandler {

	Q_DECLARE_TR_FUNCTIONS(XHToastHandler)

	public:
		XHToastHandler(void) {}

		void show(void);

	private:
		const QString iconPath = QCoreApplication::applicationDirPath() + "/xhourglass.png";

};

inline void XHToastHandler::show(void) {

	QString notificationTitle = tr("Timer Expired");
	QString notificationBody = tr("Hello, your timer has just expired.");

	QString notificationCommand = "notify-send -t 5000 \"" + notificationTitle + "\" \"" + notificationBody + "\" -i " + iconPath;

	system(notificationCommand.toStdString().c_str());

}

#endif // XHTOASTHANDLER_LINUX_H

