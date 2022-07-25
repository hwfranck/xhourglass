#include <QApplication>

#include "ui/mainwindow.h"

int main(int argc, char **argv){

    QApplication app(argc, argv);

	QCoreApplication::setOrganizationName("hwfranck");
	QCoreApplication::setOrganizationDomain("hwfranck.com");
	QCoreApplication::setApplicationName("xHourglass");

    MainWindow  window;
    window.show();

    return app.exec();
}