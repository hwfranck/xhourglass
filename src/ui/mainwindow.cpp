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
#include <QRegularExpression>
#include <QStyle>
#include <QTextStream>

#ifdef Q_OS_WIN
	#include <QWinTaskbarProgress>
	#include <QWinTaskbarButton>
#endif

MainWindow::MainWindow() : timerStarted(false), timerPaused(false), hovering(false), beepStatus(false) {

	loadSettings();

    enteredText = "";

    beepSound = new QSound(":/sound/BeepNormal.wav", this);

	#ifdef Q_OS_WIN
		tBarButton = new QWinTaskbarButton(this);
	#endif

    appTranslator.load(QString("xhourglass_lang_%1.qm").arg(currentLanguage), ":/translations");

    qApp->installTranslator(&appTranslator);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0,60);
    progressBar->setValue(0);
    progressBar->setMinimumHeight(minWindowHeight);
    progressBar->setTextVisible(false);
    progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    taskNameEdit = new QLineEdit;
	taskNameEdit->setObjectName("taskNameEdit");
    taskNameEdit->setAlignment(Qt::AlignCenter);
	taskNameEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    remainingEdit = new QLineEdit;
	remainingEdit->setText("18m");
	remainingEdit->setObjectName("remainingEdit");
    remainingEdit->setAlignment(Qt::AlignCenter);
    connect(remainingEdit, &QLineEdit::returnPressed, this, &MainWindow::startClicked);
	remainingEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

	horizontalLayout = new QHBoxLayout;
	buttonsLayout = new QHBoxLayout;
	verticalLayout = new QVBoxLayout;

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

	buttonsList.append(startButton);
	buttonsList.append(pauseButton);
	buttonsList.append(resumeButton);
	buttonsList.append(stopButton);

    elapsedLabel = new QLabel;
	elapsedLabel->setAlignment(Qt::AlignCenter);

    elapsedLabel->hide();
    pauseButton->hide();
    resumeButton->hide();
    stopButton->hide();

	buttonsLayout->setSpacing(defaultButtonsSpacing);

	buttonsLayout->addWidget(elapsedLabel);
	buttonsLayout->addWidget(startButton);
	buttonsLayout->addWidget(pauseButton);
	buttonsLayout->addWidget(resumeButton);
	buttonsLayout->addWidget(stopButton);

	buttonsLayout->insertStretch( 0, 1);
	buttonsLayout->insertStretch(-1, 1);

	verticalLayout->addWidget(taskNameEdit);
	verticalLayout->addWidget(remainingEdit);
	verticalLayout->addLayout(buttonsLayout);

	verticalLayout->insertStretch(0, 1);
	verticalLayout->insertStretch(-1, 1);

	horizontalLayout->addLayout(verticalLayout);

	horizontalLayout->insertSpacing(0, 1);
	horizontalLayout->insertSpacing(-1, 1);

    backgroundLabel = new QLabel;
    backgroundLabel->setLayout(horizontalLayout);

	connect(remainingEdit, &QLineEdit::textChanged, this, &MainWindow::scaleLineEdits);

    pbarLayout = new QVBoxLayout;
    pbarLayout->addWidget(backgroundLabel);
    pbarLayout->setContentsMargins(createMargins(defaultContentsMargins));

    progressBar->setLayout(pbarLayout);

    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(progressBar);
    mainLayout->setMargin(0);

    setLayout(mainLayout);

    remainingEdit->setFocus();
    remainingEdit->selectAll();

    remTimer = new XHTimer(1, this);
    connect(remTimer, &XHTimer::TimerChanged, this, &MainWindow::updateProgress);

    createActions();
    createContextMenu();
    retranslateUi(this);

	setWindowIcon(QIcon(":/xhourglass_icon"));

	setMinimumSize(minWindowWidth, minWindowHeight);
    resize(minWindowWidth, minWindowHeight);
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
	noSoundAction->setChecked(!beepStatus);
    beepAction->setCheckable(true);
    beepAction->setChecked(beepStatus);

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
		resizeAppLayout();
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
	appSettings.setValue("beep", beepAction->isChecked());
	appSettings.setValue("language", currentLanguage);
}

void MainWindow::loadSettings(){
	beepStatus = appSettings.value("beep", false).toBool();
	currentLanguage = appSettings.value("language", "").toString();

	if(currentLanguage.isEmpty()){
		currentLanguage = QLocale::system().name();
	}
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

			#ifdef Q_OS_WIN
				tBarProgress->setValue(newValue);
			#endif

            progressBar->update();

            remainingEdit->setText( remTimer->getRemainingTime().getFormatted() );

            break;

        case TimerState::TimerJustExpired :

                newValue = progressBar->value() + 1;
                progressBar->setValue(newValue);
                progressBar->update();

				#ifdef Q_OS_WIN
					tBarProgress->setValue(newValue);
				#endif

                styleWidgets();

                pauseButton->hide();
                stopButton->hide();

                elapsedLabel->setText( remTimer->getElapsed().getFormatted() );
                elapsedLabel->show();

                remainingEdit->setText(tr("Timer Expired"));

				#ifdef Q_OS_WIN
					tBarProgress->stop();
				#endif

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
	//	TODO: add themes
}

void MainWindow::pauseClicked(){

    remTimer->pause();
    disconnect(remTimer, &XHTimer::TimerChanged, this, &MainWindow::updateProgress);

	pauseButton->hide();
    resumeButton->show();

	#ifdef Q_OS_WIN
		tBarProgress->pause();
	#endif
}

void MainWindow::resumeClicked(){

    connect(remTimer, &XHTimer::TimerChanged, this, &MainWindow::updateProgress);
    remTimer->resume();

	resumeButton->hide();
    pauseButton->show();

	#ifdef Q_OS_WIN
		tBarProgress->resume();
	#endif
}

void MainWindow::stopClicked(){

	//	Bring the timer back into the correct state before proceeding
    if(remTimer->getTimerState() == TimerState::TimerExpired
        || remTimer->getTimerState() == TimerState::TimerJustExpired ){

        elapsedLabel->hide();
    } else if (remTimer->getTimerState() == TimerState::TimerPaused){
		resumeClicked();
	}

    remTimer->stop();

    progressBar->setValue(0);

	#ifdef Q_OS_WIN
		tBarProgress->setValue(0);
	#endif

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

	#ifdef Q_OS_WIN
		if(tBarProgress->isStopped() || tBarProgress->isPaused()){
			tBarProgress->resume();
		}

		tBarProgress->setRange(0, enteredDuration.getTotalSecs());
		tBarProgress->reset();
	#endif

    startButton->hide();
    pauseButton->show();
    stopButton->show();

    remTimer->setDuration(enteredDuration.getTotalSecs());
    remTimer->start();

    styleWidgets();
}

void MainWindow::styleWidgets(){

	QString sheet = readFile(":/styles/style-light-base.qss");

	if (remTimer->getTimerState() == TimerState::TimerJustExpired
		|| remTimer->getTimerState() == TimerState::TimerExpired) {

		QString expiredSheet = readFile(":/styles/style-light-expired.qss");
		sheet.append(expiredSheet);
	}

	qApp->setStyleSheet(sheet);

	resizeAppLayout();
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

	// Do not make adjustments when restoring a minimized window
	if(event && !event->spontaneous()){
		#ifdef Q_OS_WIN
			tBarButton->setWindow(this->windowHandle());

			tBarProgress = tBarButton->progress();
			tBarProgress->setVisible(true);
			tBarProgress->setRange(0,10);
			tBarProgress->setValue(0);
		#endif

		styleWidgets();
    }

    event->accept();
}

void MainWindow::resizeEvent(QResizeEvent *event){

	resizeAppLayout();

	event->accept();
}

void MainWindow::resizeAppLayout(){

	scaleLineEdits();

	float scaleFactorW(1.0f), scaleFactorH(1.0f), scaleFactor(1.0f);

	scaleFactorW = 1.0 * width() / minWindowWidth;
	scaleFactorH = 1.0 * height() / minWindowHeight;
	scaleFactor = std::min(scaleFactorW, scaleFactorH);

	for (QList<QPushButton*>::iterator it = buttonsList.begin();
		it != buttonsList.end();
		++it) {

		QFont font = (*it)->font();

		font.setPointSizeF(defaultTaskNameSize * scaleFactor);
		(*it)->setFont(font);
	}

	buttonsLayout->setSpacing( defaultButtonsSpacing * scaleFactor);

	resizeElapsedLabel(scaleFactor);

	pbarLayout->setContentsMargins(createMargins(defaultContentsMargins * scaleFactor));
}

void MainWindow::scaleLineEdits(){

	resizeLineEdit(remainingEdit, defaultRemainingSize);
	resizeLineEdit(taskNameEdit, defaultTaskNameSize);
}

void MainWindow::resizeLineEdit(QLineEdit* lineEdit, const float defaultSize){

	int lineEditWidth = lineEdit->width();
	float scaleFactorW(1.0f), scaleFactorH(1.0f), scaleFactor(1.0f);

	scaleFactorW = 1.0 * width() / minWindowWidth;
	scaleFactorH = 1.0 * height() / minWindowHeight;
	scaleFactor = std::min(scaleFactorW, scaleFactorH);

	float finalPointSize(defaultSize * scaleFactor);

	QString textRef = lineEdit->text();

	if (textRef.isEmpty()) {
		textRef = lineEdit->placeholderText();
	}

	QFont font = lineEdit->font();

	font.setPointSizeF(finalPointSize);
	lineEdit->setFont(font);

	QFontMetrics fmt = lineEdit->fontMetrics();
	int boundingWidth = (fmt.boundingRect(textRef)).width();

	while (boundingWidth >= lineEditWidth && finalPointSize > 0.5) {
		finalPointSize -= 0.5;
		font.setPointSizeF(finalPointSize);
		lineEdit->setFont(font);
		fmt = lineEdit->fontMetrics();
		boundingWidth = (fmt.boundingRect(textRef)).width();
	}

	//	make sure the descender is shown
	int boundingHeight = (fmt.boundingRect(textRef)).height();
	lineEdit->setFixedHeight(boundingHeight);
}

void MainWindow::resizeElapsedLabel(float scaleFactor){

	QString elapsedLabelText(elapsedLabel->text());
	QFont fontLabel = elapsedLabel->font();
	int refWidthLabel = remainingEdit->width();
	float finalPointSize = defaultTaskNameSize * scaleFactor;

	fontLabel.setPointSizeF(finalPointSize);
	elapsedLabel->setFont(fontLabel);

	QFontMetrics fmtLabel = elapsedLabel->fontMetrics();
	int boundingWidth = (fmtLabel.boundingRect(elapsedLabelText)).width();

	while (boundingWidth >= refWidthLabel) {
		finalPointSize -= 0.5;
		fontLabel.setPointSizeF(finalPointSize);
		elapsedLabel->setFont(fontLabel);
		fmtLabel = elapsedLabel->fontMetrics();
		boundingWidth = (fmtLabel.boundingRect(elapsedLabelText)).width();
	}
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
