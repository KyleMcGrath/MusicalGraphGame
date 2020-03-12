#ifndef SUMMARY_H
#define SUMMARY_H

#include <QWidget>
#include <include/view/main/graphicsscene.h>
#include <include/view/main/summary/bar.h>
#include <QJsonObject>
#include <QGraphicsScene>
#include <QJsonArray>
#include <QGraphicsView>
#include <QApplication>
#include <QScreen>
#include <QGraphicsProxyWidget>
#include <QSettings>
#include <cmath>

#define DEBUG

namespace Ui {
class Summary;
}

class Summary : public QWidget
{
    Q_OBJECT

public:
    explicit Summary(QWidget *parent = nullptr, QJsonObject gameResults = QJsonObject());
    ~Summary();
    void positionItems();

signals:
    void play();

private:
    Ui::Summary *ui;

    int order = 0;
    int modulus = 0;
    int moves = 0;
    int duration = 0;
    int minimumMoves = 0;
    int charliesMoves = 0;
    double s = 0;

    Bar *yourBar;
    Bar *parBar;


private slots:
    void graphicsClicked(const QPointF& pos);

};

#endif // SUMMARY_H
