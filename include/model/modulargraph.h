#ifndef MODULARGRAPH_H
#define MODULARGRAPH_H

#include <include/view/graph/node.h>
#include <QWidget>
#include <memory>
#include <vector>
#include <chrono>
#include <random>
#include <QScreen>
#include <QPropertyAnimation>
#include <QSettings>
#include <include/view/main/graphicsscene.h>
#include "include/view/main/scoreboard.h"
#include "include/view/main/movmodboard.h"
#include <include/view/main/graphicsview.h>
#include <QJsonObject>
#include <include/view/graph/edgeset.h>
#include <QCryptographicHash>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <set>
#include <QOpenGLWidget>
#include "include/model/mediastatics.h"
#include <QTcpSocket>
#include <include/view/main/progressbar.h>
#include "include/view/clock/clock.h"
#include "include/view/main/sboard.h"

#define DEBUG
//#define BACKEND_FUNCTIONALITY

namespace Ui {
class ModularGraph;
}

class ModularGraph : public QWidget
{
    Q_OBJECT
public:
    explicit ModularGraph(QWidget *parent = nullptr);
    ModularGraph(ModularGraph&);
    ~ModularGraph();

    void init();
    void init(int modulus, int order);
    bool gameWon();
    int getMoves();
    int getTarget();
    void reset();

    void emitModulusChanged();
    void emitTargetChanged();
    void emitMovesChanged();
    void emitValueChanged();
    void emitScoreChanged();
    void emitProblemIndexChanged();
    void emitNote(int value);
    void emitDelta(int value);
    void emitStepSignal(int steps);
    void advanceProgressBar();

    void addClock();
    void removeClock();
    void initTime();

    Scoreboard *getScoreboard() const;

signals:
    void modulusChanged(int);
    void targetChanged(int);
    void movesChanged(int);
    void valueChanged(int);
    void scoreChanged(double);
    void problemIndexChanged(int);
    void gameOver(QJsonObject);
    void displayMenu();
    void note(int);
    void delta(int);
    void stepSignal(int);

public slots:
    void startMultiPlayerGame();
    void startSinglePlayerGame();

private:
    Ui::ModularGraph *ui;
    class Impl;
    std::unique_ptr<Impl> PImpl;
    Scoreboard *scoreboard = nullptr;
    MovModBoard *movmodboard = nullptr;
    ProgressBar *progressBar = nullptr;
    SBoard *sBoard = nullptr;
    Clock *clock = nullptr;
    void endGame(QJsonObject);

private slots:
    void graphicsClicked(const QPointF&);
    void emitDisplayMenu();

};

#endif // MODULARGRAPH_H
