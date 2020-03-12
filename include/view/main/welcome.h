#ifndef WELCOME_H
#define WELCOME_H

#include <QWidget>
#include <include/view/main/about.h>
#include <include/view/main/graphicsscene.h>
#include <QGraphicsPixmapItem>
#include <QSettings>
#include "include/view/clock/clock.h"

//#define DEBUG

#define FINAL_STAGE 12

namespace Ui {
class Welcome;
}

class Welcome : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int stage READ stage WRITE setStage)

public:
    explicit Welcome(QWidget *parent = nullptr);
    ~Welcome();

    int stage();
    void setStage(int stage);

signals:
    void play();
    void delta(int);
    void modulusChanged(int);

private:
    Ui::Welcome *ui;
    About *about = nullptr;
    QGraphicsProxyWidget *charlie;
    int stage_m = 0;
    QGraphicsTextItem *text1;

    void addClock();
    void switchToTutorial();
    Clock *clock;

    bool tutorialStarted = false;
    bool tutorialFinished = false;
    bool tutorial = false;

private slots:
    void graphicsClicked(const QPointF& pos);

};

#endif // WELCOME_H
