#include "include/view/main/summary.h"
#include "ui_summary.h"

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

Summary::Summary(QWidget *parent, QJsonObject gameResults) :
    QWidget(parent),
    ui(new Ui::Summary)
{
#ifdef DEBUG
        cerr << "Construct Welcome" << endl;
#endif
    ui->setupUi(this);
    GraphicsScene *scene = new GraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(scene);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor::fromRgb(222,250,254,205));

    scene->setBackgroundBrush(brush);

    connect(scene, SIGNAL(click(const QPointF&)), this, SLOT(graphicsClicked(const QPointF&)));
    auto end = gameResults.end();

    if(gameResults.isEmpty()
            || gameResults.find(QObject::tr("order")) == end
            || gameResults.find(QObject::tr("modulus")) == end
            || gameResults.find(QObject::tr("moves")) == end
            || gameResults.find(QObject::tr("duration")) == end
            || gameResults.find(QObject::tr("minimumMoves")) == end
            || gameResults.find(QObject::tr("melody")) == end) return;

    if(gameResults.find(QObject::tr("order"))->isDouble())
        order = gameResults.find(QObject::tr("order"))->toInt();

    if(gameResults.find(QObject::tr("modulus"))->isDouble())
        modulus = gameResults.find(QObject::tr("modulus"))->toInt();

    if(gameResults.find(QObject::tr("moves"))->isDouble())
        moves = gameResults.find(QObject::tr("moves"))->toInt();

    if(gameResults.find(QObject::tr("duration"))->isDouble())
        duration = gameResults.find(QObject::tr("duration"))->toInt();

    if(gameResults.find(QObject::tr("minimumMoves"))->isDouble())
        minimumMoves = gameResults.find(QObject::tr("minimumMoves"))->toInt();

    if(gameResults.find(QObject::tr("s"))->isDouble())
        s = gameResults.find(QObject::tr("s"))->toDouble();

//    QFont font("Fruktur");

#ifdef DEBUG
        cerr << "Finish constructing Welcome" << endl;
#endif

}

Summary::~Summary()
{
#ifdef DEBUG
    cerr << "Destroy Summary" << endl;
#endif

    delete ui;

#ifdef DEBUG
    cerr << "Finish destroying Summary" << endl;
#endif
}

void Summary::positionItems()
{
    QPoint p = QApplication::primaryScreen()->availableGeometry().topLeft();

    QGraphicsScene *scene = ui->graphicsView->scene();

    int x_shift = 60;
    int y_shift = 50;

    double yourBarMoveS = double(moves)/double(minimumMoves);
    double yourBarDurS = double(duration)/minimumMoves;
    double charlieBarMoveS = double(charliesMoves)/double(minimumMoves);
    double charlieBarDurS = 1.1*double(charliesMoves)/double(minimumMoves);
    double max = yourBarMoveS+yourBarDurS;
    max = max<charlieBarMoveS+charlieBarDurS ? charlieBarMoveS+charlieBarDurS : max;
    max = max<2.0 ? 2.0 : max;

    yourBar = new Bar(nullptr, yourBarMoveS, yourBarDurS, max);
    scene->addItem(yourBar);
    yourBar->setPos(scene->views().first()->mapToScene(48+x_shift,p.y()+10+y_shift));
    yourBar->setColor(COLOR::RED);

    parBar = new Bar(nullptr, 1, 1, max);
    scene->addItem(parBar);
    parBar->setPos(scene->views().first()->mapToScene(48+x_shift,p.y()+110+y_shift));
    parBar->setColor(COLOR::BLUE);
    parBar->setLegend();

    QFont font("Roboto");

    auto title = scene->addText(tr("Game Review"));
    title->setPos(2,1);
    title->setFont(font);
    title->setScale(2);

    QSettings settings(tr("MusicalGraph"), tr("MusicalGraph"), parent());

    QString gameVersion = settings.value("gameVersion").toString();

    int gameVersionTrials = 0;
    if(settings.contains(gameVersion+tr("Trials")))
        gameVersionTrials = settings.value(gameVersion+tr("Trials")).toInt();

    double gameVersionMu = s;
    if(settings.contains(gameVersion+tr("00S")))
        gameVersionMu = settings.value(gameVersion+tr("00S")).toDouble();

    double mu = s;
    if(settings.contains(tr("00S")))
        mu = settings.value(tr("00S")).toDouble();

    int trials = 0;
    if(settings.contains(tr("trials")))
        trials = settings.value(tr("trials")).toInt();

    char *trials_formatted = new char[4];
    snprintf(trials_formatted, 4, "%d", trials);
    auto trialsItem = scene->addText(tr("Total Trials: ")+QString::fromStdString(trials_formatted));
    trialsItem->setPos(325,86);
    trialsItem->setScale(0.5);

    char *mu_formatted = new char[4];
    snprintf(mu_formatted, 4, "%2g", mu);
    auto muItem = scene->addText(tr("Average Score: ")+QString::fromStdString(mu_formatted));
    muItem->setPos(325,98);
    muItem->setScale(0.5);

    char *gameVersionTrials_formatted = new char[4];
    snprintf(gameVersionTrials_formatted, 4, "%d", gameVersionTrials);
    auto gameVersionTrialsItem = scene->addText(tr("Game Version Trials: ")+QString::fromStdString(gameVersionTrials_formatted));
    gameVersionTrialsItem->setPos(325,126);
    gameVersionTrialsItem->setScale(0.5);

    char *gameVersionMu_formatted = new char[4];
    snprintf(gameVersionMu_formatted, 4, "%2g", gameVersionMu);
    auto gameVersionMuItem = scene->addText(tr("Version Average Score: ")+QString::fromStdString(gameVersionMu_formatted));
    gameVersionMuItem->setPos(325,138);
    gameVersionMuItem->setScale(0.5);

    ui->graphicsView->scale(1.35, 1.35);
    ui->graphicsView->centerOn(p);

}

void Summary::graphicsClicked(const QPointF& pos)
{
    Q_UNUSED(pos);
#ifdef DEBUG
    cerr << "Play!" << endl;
#endif
    emit play();
    close();
}
