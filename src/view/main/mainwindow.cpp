// Produced by Kyle McGrath in 2018
// All rights reserved.

#include "include/view/main/mainwindow.h"
#include "ui_mainwindow.h"

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
#ifdef DEBUG
        cerr << "Construct MainWindow" << endl;
#endif
    ui->setupUi(this);

    Welcome *welcome = new Welcome(this);
    setCentralWidget(welcome);

    QSettings settings(tr("MusicalGraph"), tr("MusicalGraph"), this);
    if(settings.contains(tr("size")))
        resize(settings.value(tr("size")).toSize());
    if(settings.contains(tr("pos")))
        move(settings.value(tr("pos")).toPoint());

    connect(welcome, SIGNAL(play()), this, SLOT(newGame()));

    QFontDatabase::addApplicationFont(":/res/Fruktur-Regular.ttf");
    QFontDatabase::addApplicationFont(":/res/Roboto-Bold.ttf");

    musicalGraph = nullptr;

    silence = QMediaContent(QUrl("qrc:/res/notes/silence.wav"));
    mc0 = QMediaContent(QUrl("qrc:/res/notes/0.wav"));
    mc1 = QMediaContent(QUrl("qrc:/res/notes/1.wav"));
    mc2 = QMediaContent(QUrl("qrc:/res/notes/2.wav"));
    mc3 = QMediaContent(QUrl("qrc:/res/notes/3.wav"));
    mc4 = QMediaContent(QUrl("qrc:/res/notes/4.wav"));
    mc5 = QMediaContent(QUrl("qrc:/res/notes/5.wav"));
    mc6 = QMediaContent(QUrl("qrc:/res/notes/6.wav"));
    mc7 = QMediaContent(QUrl("qrc:/res/notes/7.wav"));
    mc8 = QMediaContent(QUrl("qrc:/res/notes/8.wav"));
    mc9 = QMediaContent(QUrl("qrc:/res/notes/9.wav"));
    mc10 = QMediaContent(QUrl("qrc:/res/notes/10.wav"));
    mc11 = QMediaContent(QUrl("qrc:/res/notes/11.wav"));

    player.setAudioRole(QAudio::GameRole);
    player.setMuted(false);

#ifdef DEBUG
        cerr << "Finish constructing MainWindow" << endl;
#endif
}

MainWindow::~MainWindow()
{
#ifdef DEBUG
        cerr << "Destroy MainWindow" << endl;
#endif
    QSettings settings(tr("MusicalGraph"), tr("MusicalGraph"), this);
    settings.setValue(tr("size"), QVariant(size()));
    settings.setValue(tr("pos"), QVariant(pos()));
    delete ui;
    if(musicalGraph != nullptr) delete musicalGraph;
}

void MainWindow::newGame()
{
#ifdef DEBUG
        cerr << "In void MainWindow::on_actionNew_Game_triggered()" << endl;
#endif
    initGame();
}

void MainWindow::endGame()
{
#ifdef DEBUG
        cerr << "In void MainWindow::on_actionEnd_Game_triggered()" << endl;
#endif

    Welcome *welcome = new Welcome(this);
    connect(welcome, SIGNAL(play()), this, SLOT(newGame()));
    setCentralWidget(welcome);

}

void MainWindow::exit()
{
#ifdef DEBUG
        cerr << "In void MainWindow::on_actionExit_triggered()" << endl;
#endif
    close();
}

void MainWindow::copy()
{
#ifdef DEBUG
        cerr << "In void MainWindow::on_actionCopy_triggered()" << endl;
#endif

}

void MainWindow::about()
{
#ifdef DEBUG
        cerr << "In void MainWindow::on_actionAbout_triggered()" << endl;
#endif
    About *about = new About();
    about->show();
}

void MainWindow::initGame()
{
#ifdef DEBUG
        cerr << "In void MainWindow::initGame()" << endl;
#endif

    if(musicalGraph == nullptr) {
        musicalGraph = new ModularGraph(this);
        showGame();
        musicalGraph->init();
    }
#ifdef DEBUG
        cerr << "Initializing modGraph" << endl;
#endif
    connect(musicalGraph, SIGNAL(gameOver(QJsonObject)), this, SLOT(handleGameOver(QJsonObject)));
    connect(musicalGraph, SIGNAL(note(int)), this, SLOT(receiveNote(int)));
#ifdef DEBUG
        cerr << "Showing modGraph" << endl;
#endif

}

void MainWindow::handleGameOver(QJsonObject json)
{
    Summary *summary = new Summary(this, json);
    connect(summary, SIGNAL(play()), this, SLOT(newGame()));
    setCentralWidget(summary);
    summary->positionItems();
    musicalGraph = nullptr;
}

void MainWindow::displayGameList()
{
    setCentralWidget(musicalGraph);
    GameList *gameList = new GameList(musicalGraph);
    gameList->show();
    gameList->raise();
    gameList->activateWindow();
    connect(gameList, SIGNAL(loadGame(QString)), this, SLOT(showLoading()));
    connect(gameList, SIGNAL(loadGame(QString)), musicalGraph, SLOT(loadGame(QString)));
    connect(gameList, SIGNAL(loadGame(QString)), this, SLOT(resumeGame()));
}

void MainWindow::resumeGame()
{
    initGame();
}

void MainWindow::killGame()
{
#ifdef DEBUG
        cerr << "In void MainWindow::killGame()" << endl;
#endif
    if(musicalGraph != nullptr)
        delete musicalGraph;
    musicalGraph = nullptr;
}
void MainWindow::showGame()
{
#ifdef DEBUG
        cerr << "In void MainWindow::showGame()" << endl;
#endif
    setCentralWidget(musicalGraph);
    musicalGraph->show();
    musicalGraph->raise();
    musicalGraph->activateWindow();
    QApplication::processEvents();
}

void MainWindow::receiveNote(int note)
{
    melody[2] = note;
    melody[1] = musicalGraph->getTarget();
    melody[0] = note;
#ifdef DEBUG
        cerr << "In void MainWindow::receiveNote(int note)" << endl;
#endif
    setPlaylist();
}

void MainWindow::setPlaylist()
{
    QMediaPlaylist *playlist = new QMediaPlaylist(this);
    playlist->addMedia(silence);
    for(int i = 0; i < 3; i++)
#ifdef DEBUG
    {
        cerr << "In void MainWindow::setPlaylist() melody[" << to_string(i) << "]=" << to_string(melody[i]) << endl;
#endif
        switch(melody[i] % 12) {
        case 0:
            playlist->addMedia(mc0);
            break;
        case 1:
            playlist->addMedia(mc1);
            break;
        case 2:
            playlist->addMedia(mc2);
            break;
        case 3:
            playlist->addMedia(mc3);
            break;
        case 4:
            playlist->addMedia(mc4);
            break;
        case 5:
            playlist->addMedia(mc5);
            break;
        case 6:
            playlist->addMedia(mc6);
            break;
        case 7:
            playlist->addMedia(mc7);
            break;
        case 8:
            playlist->addMedia(mc8);
            break;
        case 9:
            playlist->addMedia(mc9);
            break;
        case 10:
            playlist->addMedia(mc10);
            break;
        case 11:
            playlist->addMedia(mc11);
            break;
        }
#ifdef DEBUG
    }
#endif

    player.setPlaylist(playlist);
    player.play();
}

void MainWindow::on_actionNew_Game_triggered()
{
    killGame();
    initGame();
    showGame();
}

void MainWindow::on_actionSettings_triggered()
{
    Dialog *dialog = new Dialog(this);
    dialog->setModal(true);
    dialog->exec();
}

void MainWindow::on_actionQuit_triggered()
{
    exit();
}
