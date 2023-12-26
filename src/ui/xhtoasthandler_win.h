#ifndef XHTOASTHANDLER_WIN_H
#define XHTOASTHANDLER_WIN_H

#include "../third-party/wintoast/wintoastlib.h"

#include <QDebug>

class XHToastHandler : public WinToastLib::IWinToastHandler {

	Q_DECLARE_TR_FUNCTIONS(XHToastHandler)

	public:
		XHToastHandler(void);

		void toastActivated() const override {}
		void toastActivated(int) const {}
		void toastDismissed(WinToastLib::IWinToastHandler::WinToastDismissalReason) const override {}
		void toastFailed() const override {}

		void show(void);
};

inline XHToastHandler::XHToastHandler(void) : WinToastLib::IWinToastHandler() {

	if (!WinToastLib::WinToast::isCompatible()){
		return;
	}

	WinToastLib::WinToast::instance()->setAppName(L"xHourglass");
	const auto aumi = WinToastLib::WinToast::configureAUMI(L"hwfranck", L"xHourglass");
	WinToastLib::WinToast::instance()->setAppUserModelId(aumi);

	WinToastLib::WinToast::WinToastError toastError;
	const bool toastInitSucceeded = WinToastLib::WinToast::instance()->initialize(&toastError);
	if (!toastInitSucceeded){
		qDebug() << "Error initiliazing WinToastLib (error number : " << toastError << ")";
		return;
	}
}

inline void XHToastHandler::show(void) {

	WinToastLib::WinToastTemplate toastTemplate = WinToastLib::WinToastTemplate::Text02;
	toastTemplate.setTextField(tr("Timer Expired").toStdWString(), WinToastLib::WinToastTemplate::FirstLine);
	toastTemplate.setTextField(tr("Hello, your timer has just expired.").toStdWString(), WinToastLib::WinToastTemplate::SecondLine);

	XHToastHandler toastHandler = XHToastHandler();
	WinToastLib::WinToast::WinToastError toastError;

	const bool toastLaunched = WinToastLib::WinToast::instance()->showToast(toastTemplate, &toastHandler, &toastError);
	if (!toastLaunched){
		qDebug() << "Error while trying to display the toast (error number : " << toastError << ")";
	}
}

#endif // XHTOASTHANDLER_WIN_H
