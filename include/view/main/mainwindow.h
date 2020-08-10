// Produced by Kyle McGrath in 2018
// All rights reserved.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <include/model/modulargraph.h>
#include <include/view/main/welcome.h>
#include <include/view/main/about.h>
#include "include/view/main/summary.h"
#include <QFontDatabase>
#include "include/view/main/gamelist.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include "include/view/main/dialog.h"

//#define DEBUG

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newGame();

    void endGame();

    void exit();

    void copy();

    void about();

    void initGame();

    void handleGameOver(QJsonObject);

    void displayGameList();

    void resumeGame();

    void killGame();

    void showGame();

    void receiveNote(int);

    void on_actionNew_Game_triggered();

    void on_actionSettings_triggered();

    void on_actionQuit_triggered();

private:
    Ui::MainWindow *ui;
    ModularGraph *musicalGraph;
    int *melody = new int[3];

    QMediaPlayer *player = new QMediaPlayer(nullptr, QMediaPlayer::LowLatency);

    void setPlaylist();

    QMediaContent silence;
    QMediaContent mc0;
    QMediaContent mc1;
    QMediaContent mc2;
    QMediaContent mc3;
    QMediaContent mc4;
    QMediaContent mc5;
    QMediaContent mc6;
    QMediaContent mc7;
    QMediaContent mc8;
    QMediaContent mc9;
    QMediaContent mc10;
    QMediaContent mc11;
};

#endif // MAINWINDOW_H
