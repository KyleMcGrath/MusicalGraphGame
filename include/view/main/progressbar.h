#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QGraphicsPixmapItem>
#include <QPropertyAnimation>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QApplication>
#include <QScreen>
#include <QObject>
#include <cmath>

class ProgressBar : public QObject, public QGraphicsPixmapItem
{

//    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    ProgressBar(QGraphicsItem *parent = nullptr);

public slots:
    void moveToCorner();
    void setProgress(int level);
    int getProgress() const { return progress_m; }
    int progress_m = 1;

private:
    QPixmap  p1;
    QPixmap  p2;
    QPixmap  p3;
    QPixmap  p4;
    QPixmap  p5;
    QPixmap  p6;
    QPixmap  p7;
    QPixmap  p8;
    QPixmap  p9;
    QPixmap p10;

};

#endif // PROGRESSBAR_H
