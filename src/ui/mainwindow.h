#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../core/xhduration.h"
#include "../core/xhtimer.h"
#include "xhtoasthandler.h"

#include <string>

#include <QCoreApplication>
#include <QWidget>
#include <QTranslator>
#include <QLineEdit>
#include <QSettings>

class QTimer;
class QTime;
class QProgressBar;
class QLabel;
class QPropertyAnimation;
class QPushButton;
class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class QAction;
class QActionGroup;
class QMargins;
class QMenu;
class QSound;
class QWinTaskbarProgress;
class QWinTaskbarButton;

class MainWindow : public QWidget{

    Q_OBJECT

    public :
        MainWindow();

    private slots :
        void updateProgress();
        void pauseClicked();
        void resumeClicked();
		void scaleLineEdits();
        void stopClicked();
        void startClicked();
        void languageChanged(QAction*);
        void closeEvent(QCloseEvent *event) override;
        void closeClicked();
        void formatsClicked();
        void aboutClicked();

    protected:
        void enterEvent(QEvent *event) override;
        void leaveEvent(QEvent *event) override;
        void changeEvent(QEvent *) override;
		void resizeEvent(QResizeEvent *event) override;
        void showEvent(QShowEvent *event) override;

    private :
        void styleWidgets();
        void createActions();
        void createContextMenu();
        void loadLanguage(const QString& newLanguage);
		void loadSettings();
        void switchTranslator(QTranslator&, const QString&newLang);
		void resizeAppLayout();
		void resizeElapsedLabel(float scaleFactor);
		void resizeLineEdit(QLineEdit*, const float defaultPointSize);
        void retranslateUi(QWidget*);
        void saveSettings();
        void updateStyle();
		QString readHelp();
		QString readFile(const QString fname);

        QProgressBar *progressBar;
        QLineEdit *taskNameEdit;
        QLineEdit *remainingEdit;
        QLabel *backgroundLabel;

        XHTimer *remTimer;

		XHToastHandler notification;

        int lastEntry;
        QPushButton *startButton,
                    *pauseButton,
                    *resumeButton,
                    *stopButton;
        QVBoxLayout *mainLayout,
					*pbarLayout,
					*verticalLayout;
        QHBoxLayout *horizontalLayout,
					*buttonsLayout;
        QLabel *elapsedLabel;

		QList<QPushButton*> buttonsList;

        bool timerExpired,
            timerStarted,
            timerPaused,
            hovering,
			beepStatus;

        QMenu *soundMenu,
                *languageMenu,
                *helpMenu;

        QAction *alwayOnTopAction,
                *languageAction,
                *englishAction,
                *germanAction,
                *frenchAction,
                *spanishAction,
                *japaneseAction, // TODO: add japanese translation
                *soundAction,
                *noSoundAction,
                *beepAction,
                *closeAction,
                *firstSeparator,
                *secondSeparator,
                *formatsAction,
                *aboutAction,
                *helpAction;

        QActionGroup *languageGroup,
                    *soundGroup;

        QString currentLanguage,
                languagesPath,
                enteredText;

        QTranslator appTranslator,
                    qtTranslator;

        QSound *beepSound;

        QWinTaskbarProgress *tBarProgress;
        QWinTaskbarButton *tBarButton;

		QSettings appSettings;

		const float defaultRemainingSize = 20.0F,
			defaultTaskNameSize = 10.0F;
		const int defaultButtonsSpacing = 6,
			defaultContentsMargins = 10,
			mainMarginsLeftRight = 1;

		const int minWindowWidth = 336,
			minWindowHeight = 112;
};

#endif // MAINWINDOW_H
