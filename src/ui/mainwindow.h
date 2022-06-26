#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../core/xhduration.h"
#include "../core/xhtimer.h"

#include <string>

#include <QCoreApplication>
#include <QWidget>
#include <QTranslator>
#include <QLineEdit>

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
        void showEvent(QShowEvent *event) override;

    private :
        void styleWidgets();
        void createActions();
        void createContextMenu();
        void loadLanguage(const QString& newLanguage);
        void switchTranslator(QTranslator&, const QString&newLang);
        void retranslateUi(QWidget*);
        void saveSettings();
        void updateStyle();

        QProgressBar *progressBar;
        QLineEdit *taskNameEdit;
        QLineEdit *remainingEdit;
        QLabel *backgroundLabel;

        XHTimer *remTimer;

        int lastEntry;
        QPushButton *startButton,
                    *pauseButton,
                    *resumeButton,
                    *stopButton;
        QGridLayout *buttonsLayout,
                    *bgLayout;
        QVBoxLayout *vLayout,
                    *mainLayout,
                    *tLayout;
        QHBoxLayout *hLayout;
        QLabel *elapsedLabel;

        bool timerExpired,
            timerStarted,
            timerPaused,
            hovering;

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
};

#endif // MAINWINDOW_H
