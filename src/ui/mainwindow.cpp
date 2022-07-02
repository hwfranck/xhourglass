#include "mainwindow.h"
#include "../core/xh_utils.h"
#include "../core/parse.h"

#include <iostream>
#include <string>
#include <vector>

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QLineEdit>
#include <QTimer>
#include <QTime>
#include <QPropertyAnimation>
#include <QApplication>
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QTranslator>
#include <QSound>
#include <QCloseEvent>
#include <QMessageBox>
#include <QWinTaskbarProgress>
#include <QWinTaskbarButton>
#include <QRegularExpression>
#include <QTextStream>

MainWindow::MainWindow() : timerStarted(false), timerPaused(false), hovering(false) {

    enteredText = "";

    beepSound = new QSound(":/sound/BeepNormal.wav", this);

    tBarButton = new QWinTaskbarButton(this);

    currentLanguage = QLocale::system().name();

    appTranslator.load(QString("xhourglass_lang_%1.qm").arg(currentLanguage), ":/translations");

    qApp->installTranslator(&appTranslator);

    const int winWidth(336);
    const int winHeight(112);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0,60);
    progressBar->setValue(0);
    progressBar->setMinimumHeight(winHeight);
    progressBar->setTextVisible(false);
    progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    taskNameEdit = new QLineEdit(this);
    taskNameEdit->setAlignment(Qt::AlignCenter);
    taskNameEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    remainingEdit = new QLineEdit(this);
    remainingEdit->setAlignment(Qt::AlignCenter);
    connect(remainingEdit, &QLineEdit::returnPressed, this, &MainWindow::startClicked);
    remainingEdit->setText("18m");
    remainingEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    bgLayout = new QGridLayout;
    bgLayout->addWidget(taskNameEdit, 1, 1);
    bgLayout->addWidget(remainingEdit, 2, 1);

    bgLayout->setColumnMinimumWidth(0,2);
    bgLayout->setColumnMinimumWidth(2,2);

    bgLayout->setRowMinimumHeight(0,2);
    bgLayout->setRowMinimumHeight(4,2);

    bgLayout->setColumnStretch(0,1);
    bgLayout->setColumnStretch(1,24);
    bgLayout->setColumnStretch(2,1);

    bgLayout->setRowStretch(0,0);
    bgLayout->setRowStretch(1,1);   //  taskname
    bgLayout->setRowStretch(2,2);   //  remaining
    bgLayout->setRowStretch(3,1);   //  buttons
    bgLayout->setRowStretch(4,0);   //


    startButton = new QPushButton;
    pauseButton = new QPushButton;
    resumeButton = new QPushButton;
    stopButton = new QPushButton;

    startButton->setCursor(Qt::PointingHandCursor);
    pauseButton->setCursor(Qt::PointingHandCursor);
    resumeButton->setCursor(Qt::PointingHandCursor);
    stopButton->setCursor(Qt::PointingHandCursor);

    connect(startButton, &QPushButton::pressed, this, &MainWindow::startClicked);
    connect(pauseButton, &QPushButton::pressed, this, &MainWindow::pauseClicked);
    connect(resumeButton, &QPushButton::pressed, this, &MainWindow::resumeClicked);
    connect(stopButton, &QPushButton::pressed, this, &MainWindow::stopClicked);

    elapsedLabel = new QLabel;

    buttonsLayout = new QGridLayout;

    buttonsLayout->addWidget(elapsedLabel, 0, 2, Qt::AlignCenter);
    buttonsLayout->addWidget(startButton,0,2,Qt::AlignCenter);
    buttonsLayout->addWidget(pauseButton,0,1,Qt::AlignRight);
    buttonsLayout->addWidget(resumeButton,0,1,Qt::AlignRight);
    buttonsLayout->addWidget(stopButton,0,3, Qt::AlignLeft);
    buttonsLayout->setColumnMinimumWidth(0,10);
    buttonsLayout->setColumnMinimumWidth(2,10);
    buttonsLayout->setColumnMinimumWidth(4,10);
    buttonsLayout->setColumnStretch(0,1);
    buttonsLayout->setColumnStretch(4,1);
    bgLayout->addLayout(buttonsLayout, 3, 1);

    elapsedLabel->hide();
    pauseButton->hide();
    resumeButton->hide();
    stopButton->hide();

    backgroundLabel = new QLabel;

    backgroundLabel->setLayout(bgLayout);

    tLayout = new QVBoxLayout;
    tLayout->addWidget(backgroundLabel);

    progressBar->setLayout(tLayout);
    progressBar->setContentsMargins(2,2,2,2);

    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(progressBar);
    mainLayout->setMargin(0);

    styleWidgets();

    setLayout(mainLayout);

    remainingEdit->setFocus();
    remainingEdit->selectAll();

    remTimer = new XHTimer(1, this);
    connect(remTimer, &XHTimer::TimerChanged, this, &MainWindow::updateProgress);

    setMinimumSize(winWidth,winHeight);

    createActions();
    createContextMenu();
    retranslateUi(this);
}

void MainWindow::createActions(){

    firstSeparator = new QAction(this);
    firstSeparator->setSeparator(true);

    languageAction = new QAction(this);
    englishAction = new QAction(this);
    frenchAction = new QAction(this);
    germanAction = new QAction(this);
    spanishAction = new QAction(this);

    englishAction->setCheckable(true);
    englishAction->setData("en_US");

    frenchAction->setCheckable(true);
    frenchAction->setData("fr_FR");

    germanAction->setCheckable(true);
    germanAction->setData("de_DE");

    spanishAction->setCheckable(true);
    spanishAction->setData("es_ES");

    languageGroup = new QActionGroup(this);
    languageGroup->setExclusive(true);
    languageGroup->addAction(englishAction);
    languageGroup->addAction(frenchAction);
    languageGroup->addAction(germanAction);
    languageGroup->addAction(spanishAction);

    connect(languageGroup, SIGNAL(triggered(QAction*)), this, SLOT(languageChanged(QAction*)));

    languageMenu = new QMenu(this);
    languageMenu->addActions(QList<QAction*>()
                             << englishAction
                             << spanishAction
                             << frenchAction
							 << germanAction
							);
    languageAction->setMenu(languageMenu);

	// Add a check mark to the current language in the menu
	bool isSystemTranslationAvailable(false);
    QAction *a;
    QList<QAction*> l = languageGroup->actions();

    for(int i = 0; i < l.size(); i++){
        a = l.at(i);
        if(currentLanguage == a->data().toString()){
            a->setChecked(true);
            isSystemTranslationAvailable = true;
			break;
        }
    }

   if(!isSystemTranslationAvailable){
		englishAction->trigger();
   }

    noSoundAction = new QAction(this);
    beepAction = new QAction(this);
    noSoundAction->setCheckable(true);
    beepAction->setCheckable(true);
    beepAction->setChecked(true);

    soundGroup = new QActionGroup(this);
    soundGroup->setExclusive(true);
    soundGroup->addAction(noSoundAction);
    soundGroup->addAction(beepAction);

    soundAction = new QAction(this);
    soundMenu = new QMenu(this);
    soundMenu->addActions(QList<QAction*>()
                          << noSoundAction
                          << beepAction);
    soundAction->setMenu(soundMenu);

    helpAction = new QAction(this);
    formatsAction = new QAction(this);
    aboutAction = new QAction(this);

    helpMenu = new QMenu(this);
    helpMenu->addActions(QList<QAction*>()
                         << formatsAction
                         << aboutAction);
    helpAction->setMenu(helpMenu);

    connect(formatsAction, &QAction::triggered, this, &MainWindow::formatsClicked);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::aboutClicked);

    secondSeparator = new QAction(this);
    secondSeparator->setSeparator(true);

    closeAction = new QAction(this);
    closeAction->setShortcutVisibleInContextMenu(true);
    connect(closeAction, &QAction::triggered, this, &MainWindow::closeClicked);
}

void MainWindow::formatsClicked(){

    QMessageBox formatsBox(QMessageBox::NoIcon, tr("Supported Time formats"), "CONTENT", QMessageBox::Ok, this);

    formatsBox.setTextFormat(Qt::RichText);
	formatsBox.setText(readHelp());
    formatsBox.exec();
}

void MainWindow::aboutClicked(){

    QMessageBox aboutBox(QMessageBox::NoIcon, tr("About xHourglass"), "CONTENT", QMessageBox::Ok, this);

    aboutBox.setTextFormat(Qt::RichText);
    aboutBox.setText("This is a cross-platform reimplementation of 'Hourglass by Chris Dziemborowicz.'"
                   "<br/> Available at <a href='https://github.com/dziemborowicz/hourglass'>https://github.com/dziemborowicz/hourglass</a>"
                   "<br/><br/>Source code at <a href='https://github.com/hwfranck/xHourglass'>https://github.com/hwfranck/xHourglass</a>"
                   );
    aboutBox.exec();
}

void MainWindow::changeEvent(QEvent *event){

    if(event && event->type() == QEvent::LanguageChange){
        retranslateUi(this);
    }

    QWidget::changeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event){

	saveSettings();
	event->accept();
	// TODO : Do not close unless TimerState::TimerStopped
}

void MainWindow::closeClicked(){

    saveSettings();
    qApp->quit();
}

void MainWindow::saveSettings(){
//  TODO : Implement saveSettings()
}

void MainWindow::createContextMenu(){

    progressBar->addActions(QList<QAction*>()
                            << firstSeparator
                            << languageAction
                            << soundAction
                            <<  helpAction
                            << secondSeparator
                            << closeAction);

    progressBar->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void MainWindow::languageChanged(QAction* action){

    if(action){
        loadLanguage(action->data().toString());
    }
}

void MainWindow::loadLanguage(const QString &newLanguage){

    if(currentLanguage != newLanguage){
        currentLanguage = newLanguage;
        QLocale locale = QLocale(newLanguage);
        QLocale::setDefault(locale);
        switchTranslator(appTranslator, QString("xhourglass_lang_%1.qm").arg(newLanguage));
    }
}

void MainWindow::switchTranslator(QTranslator& translator, const QString& filename){
    qApp->removeTranslator(&translator);

    if(translator.load(filename, ":/translations")){
        qApp->installTranslator(&translator);
    }
}

void MainWindow::updateProgress(){

    int newValue(0),
            temp(0);
    XHDuration t(temp);

    switch( remTimer->getTimerState() ){

        case TimerState::TimerRunning :
            newValue = progressBar->value() + 1;
            progressBar->setValue(newValue);
            tBarProgress->setValue(newValue);
            progressBar->update();

            remainingEdit->setText( remTimer->getRemainingTime().getFormatted() );

            break;

        case TimerState::TimerJustExpired :

                newValue = progressBar->value() + 1;
                progressBar->setValue(newValue);
                progressBar->update();
                tBarProgress->setValue(newValue);

                updateStyle();

                pauseButton->hide();
                stopButton->hide();

                elapsedLabel->setText( remTimer->getElapsed().getFormatted() );
                elapsedLabel->show();

                remainingEdit->setText(tr("Timer Expired"));

                tBarProgress->stop();

                if(beepAction->isChecked()){
                    beepSound->play();
                }
            break;

        case TimerState::TimerExpired :
            {
                QString elapsedText = QString(tr("%1 ago")).arg( remTimer->getElapsed().getFormatted() );

                elapsedLabel->setText( elapsedText );
                break;
            }
        case TimerState::TimerPaused :
            break;

        default:
            break;
    }
}

void MainWindow::updateStyle(){

    QString s;

    if(remTimer->getTimerState() == TimerState::TimerJustExpired){
        s = QString("QProgressBar::chunk {"
                    "background-color: rgb(199, 80, 80);"
                    "}"
                    ""
                    "QProgressBar {"
                    "border: 0px solid rgb(255, 0, 0);"
                    "background-color: rgb(238, 238, 238);"
                    "}"
                    "QPushButton {"
                    "color: rgb(0, 102, 204);"
                    "border-style: none;"
                    "}"
                    ""
                    "QPushButton:hover {"
                    "color: rgb(255, 0, 0);"
                    "}"
                  "");
    }else {
        s = QString("QProgressBar::chunk {"
                    "background-color: rgb(54, 101, 179);"
                    "}"
                    ""
                    "QProgressBar {"
                    "border: 0px solid rgb(255, 0, 0);"
                    "background-color: rgb(238, 238, 238);"
                    "}"
                    "QPushButton {"
                    "color: rgb(0, 102, 204);"
                    "border-style: none;"
                    "}"
                    ""
                    "QPushButton:hover {"
                    "color: rgb(255, 0, 0);"
                    "}"
                  "");
    }

    qApp->setStyleSheet(s);
}

void MainWindow::pauseClicked(){

    remTimer->pause();
    disconnect(remTimer, &XHTimer::TimerChanged, this, &MainWindow::updateProgress);

	pauseButton->hide();
    resumeButton->show();

    tBarProgress->pause();
}

void MainWindow::resumeClicked(){

    connect(remTimer, &XHTimer::TimerChanged, this, &MainWindow::updateProgress);
    remTimer->resume();

	resumeButton->hide();
    pauseButton->show();

    tBarProgress->resume();
}

void MainWindow::stopClicked(){

    if(remTimer->getTimerState() == TimerState::TimerExpired
        || remTimer->getTimerState() == TimerState::TimerJustExpired ){

        elapsedLabel->hide();
    }

    remTimer->stop();

    progressBar->setValue(0);
    tBarProgress->setValue(0);

    remainingEdit->setText(enteredText);

    pauseButton->hide();
    stopButton->hide();
    startButton->show();
}

void MainWindow::startClicked(){

    enteredText = remainingEdit->text();

    std::string durationText = remainingEdit->text().toLower().toStdString();
    int durationInt = processInput(durationText);
    XHDuration enteredDuration(durationInt);

    int tSecs = enteredDuration.getTotalSecs();

    if(tSecs == 0)
        return;

    progressBar->setRange(0, enteredDuration.getTotalSecs());
    progressBar->setValue(0);

    tBarProgress->setRange(0, enteredDuration.getTotalSecs());
    tBarProgress->reset();
    tBarProgress->setValue(0);

    startButton->hide();
    pauseButton->show();
    stopButton->show();

    remTimer->setDuration(enteredDuration.getTotalSecs());
    remTimer->start();

    updateStyle();
}

void MainWindow::styleWidgets(){

    taskNameEdit->setStyleSheet("QLineEdit {"
                                "color: #808081;"
                                "border-style: none;"
                                "font-size : 10px;"
                                "}"
                                ""
                                "QLineEdit:hover{"
                                "border: 1px solid #569de5;"
                                "}"
                                ""
                                "QLineEdit:focus{"
                                "border: 1px solid #569de5;"
                                "}"
                                "");

    remainingEdit->setStyleSheet("QLineEdit {"
                                 "color : black;"
                                 "border-style: none;"
                                 "font-size : 20px;"
                                 "}"
                                 ""
                                 "QLineEdit:hover{"
                                 "border: 1px solid #569de5;"
                                 "}"
                                 ""
                                 "QLineEdit:focus{"
                                 "border: 1px solid #569de5;"
                                 "}"
                                 "");

    backgroundLabel->setStyleSheet("*{"
                                   "background-color : white;"
                                   "}");

    QString s("QProgressBar::chunk {"
                "background-color: rgb(54, 101, 179);"
                "}"
                ""
                "QProgressBar {"
                "border: 0px solid rgb(255, 0, 0);"
                "background-color: rgb(238, 238, 238);"
                "}"
                "QPushButton {"
                "color: rgb(0, 102, 204);"
                "border-style: none;"
                "}"
                ""
                "QPushButton:hover {"
                "color: rgb(255, 0, 0);"
                "}"
              "");
    qApp->setStyleSheet(s);
}

void MainWindow::enterEvent(QEvent*){

    hovering = true;
    if(remTimer->getTimerState() == TimerState::TimerExpired){
        elapsedLabel->hide();
        stopButton->show();
    }
}

void MainWindow::leaveEvent(QEvent*){

    hovering = false;
    if(remTimer->getTimerState() == TimerState::TimerExpired){
        elapsedLabel->show();
        pauseButton->hide();
        stopButton->hide();
    }
}

void MainWindow::showEvent(QShowEvent *event){
    tBarButton->setWindow(this->windowHandle());

    tBarProgress = tBarButton->progress();
    tBarProgress->setVisible(true);
    tBarProgress->setRange(0,10);
    tBarProgress->setValue(0);

    event->accept();
}

void MainWindow::retranslateUi(QWidget*){

    taskNameEdit->setPlaceholderText(tr("Click to enter the title"));
    remainingEdit->setPlaceholderText(tr("Click to enter the duration"));

    startButton->setText(tr("Start"));
    pauseButton->setText(tr("Pause"));
    resumeButton->setText(tr("Resume"));
    stopButton->setText(tr("Stop"));

    setWindowTitle(tr("xHourglass"));

    languageAction->setText(tr("&Language"));
    englishAction->setText(tr("&English"));
    spanishAction->setText(tr("&Español"));
    frenchAction->setText(tr("&Français"));
    germanAction->setText(tr("&Deutsch"));

    noSoundAction->setText(tr("No Sound"));
    beepAction->setText(tr("Beep"));
    beepAction->setToolTip(tr("Emits a beep when timer finishes"));

    soundAction->setText(tr("Sound"));
    helpAction->setText(tr("Help"));
    formatsAction->setText(tr("Formats"));
    aboutAction->setText(tr("About"));
    closeAction->setText(tr("&Quit"));
    closeAction->setShortcut(QString(tr("Alt+F4")));
}

QString MainWindow::readHelp(){

	QString nameHelpFile = QString(":sup_formats/format_%1").arg(currentLanguage);

	return readFile(nameHelpFile);
}

QString MainWindow::readFile(const QString fname){

	QString content;

	QFile mFile(fname);
	if(mFile.open(QIODevice::ReadOnly)){

		QTextStream inStream(&mFile);
		content = inStream.readAll();

		mFile.close();
	}

	return content;
}
