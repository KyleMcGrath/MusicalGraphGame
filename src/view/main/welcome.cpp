#include "include/view/main/welcome.h"
#include "ui_welcome.h"

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

Welcome::Welcome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Welcome)
{
#ifdef DEBUG
        cerr << "Construct Welcome" << endl;
#endif
    ui->setupUi(this);
    GraphicsScene *scene = new GraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(scene);

    QPixmap welcome_bg(":/res/welcome-bg.png");

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor::fromRgb(222,250,254,255));

    scene->setBackgroundBrush(brush);
    scene->addPixmap(welcome_bg)->setScale(0.45);

    QPixmap welcome_logo(":/res/welcome-logo.png");

    auto logoItem = scene->addPixmap(welcome_logo);
    logoItem->setScale(0.25);
    logoItem->setPos(10, 10);

    QPixmap welcome_nodes(":/res/notice.png");
    auto nodesItem = scene->addPixmap(welcome_nodes);
    nodesItem->setScale(0.11);
    nodesItem->moveBy(335, 65);

    connect(scene, SIGNAL(click(const QPointF&)), this, SLOT(graphicsClicked(const QPointF&)));

    QSettings settings(tr("MusicalGraph"), tr("MusicalGraph"), this);
    if(settings.contains(tr("first")))
        tutorial = settings.value(tr("first")).toBool();
    else tutorial = true;


#ifdef DEBUG
        cerr << "Finish constructing Welcome" << endl;
#endif
}

Welcome::~Welcome()
{
#ifdef DEBUG
        cerr << "Destroy Welcome" << endl;
#endif
        if(about != nullptr) {
            delete about;
    #ifdef DEBUG
            cerr << "Destroy Welcome::about" << endl;
    #endif
        }
    delete ui;
#ifdef DEBUG
        cerr << "Finish destroying Welcome" << endl;
#endif
}

int Welcome::stage()
{
    return stage_m;
}

void Welcome::setStage(int stage)
{
    stage_m = stage;
    QGraphicsScene *scene = ui->graphicsView->scene();
    switch(stage_m) {
    case 0:
    {
        scene->clear();
        text1 = scene->addText(tr("Musical Math Game"));
        text1->setDefaultTextColor(Qt::black);
        text1->setScale(2);
        auto text2 = scene->addText(tr("Imagine if clocks had many forms with different numbers of hours"));
        text2->setDefaultTextColor(Qt::black);
        text2->setScale(2);
        text2->setPos(0, 125);
        break;
    }
    case 1:
    {
        scene->clear();
        addClock();
        text1 = scene->addText(tr("Start at 0 and count by one"));
        text1->setDefaultTextColor(Qt::black);
        text1->setScale(2);
        text1->setPos(205,0);
        break;
    }
    case 2:
    {
        text1->setPlainText(tr("now the number of hours can change\nbut the goal is the same\nto have the current value reach the target value"));
        text1->setDefaultTextColor(Qt::black);
        clock->setModulus(4);
        clock->setRadianDelta();
        break;
    }
    case 5:
    {
        text1->setPlainText(tr("The clock gives the current value"));
        text1->setDefaultTextColor(Qt::black);
        clock->setModulus(7);
        clock->setRadianDelta();
        break;
    }
    case 8: {
        text1->setPlainText(tr("if you start at 8"));
        text1->setDefaultTextColor(Qt::black);
        clock->setTime(8);
        break;
    }
    case 9: {
        text1->setPlainText(tr("and travel 7h"));
        text1->setDefaultTextColor(Qt::black);
        break;
    }
    case 10: {
        text1->setPlainText(tr("you arrive at 6!  tap anywhere to play"));
        text1->setDefaultTextColor(Qt::black);
        clock->timeTravel(7);
        QSettings settings(tr("MusicalGraph"), tr("MusicalGraph"), this);
        settings.setValue(tr("first"), false);
        tutorialFinished = true;
        break;
    }
    }
}

void Welcome::addClock()
{
    clock = new Clock(nullptr);
    clock->setParent(this);
    ui->graphicsView->scene()->addItem(clock);
    QObject::connect(this, &Welcome::delta, clock, &Clock::timeTravel);
    QObject::connect(this, &Welcome::modulusChanged, clock, &Clock::setModulus);
    clock->setModulus(3);
    clock->setRadianDelta();
    clock->setTime(0);
}

void Welcome::switchToTutorial()
{
    tutorialStarted = true;
    ui->graphicsView->scale(0.6,0.6);
    QPropertyAnimation *animation = new QPropertyAnimation(this, "stage", this);
    animation->setStartValue(0);
    animation->setEndValue(FINAL_STAGE);
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->setDuration(40000);
    animation->start();

}

void Welcome::graphicsClicked(const QPointF& pos)
{
    Q_UNUSED(pos);
#ifdef DEBUG
    cerr << "Play!" << endl;
#endif
    if(tutorialFinished || !tutorial)
        emit play();
    else if(!tutorialStarted)
        switchToTutorial();
}

