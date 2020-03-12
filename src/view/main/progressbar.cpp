#include "include/view/main/progressbar.h"

#include <iostream>

ProgressBar::ProgressBar(QGraphicsItem *parent) :
    QObject(nullptr), QGraphicsPixmapItem(parent)
{
    setPixmap(p1);
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
    setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren);
    setFlag(QGraphicsItem::ItemContainsChildrenInShape);
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    setScale(0.24);
    p1 = QPixmap(":/res/progressBar/progressBar-1.png");
    p2 = QPixmap(":/res/progressBar/progressBar-2.png");
    p3 = QPixmap(":/res/progressBar/progressBar-3.png");
    p4 = QPixmap(":/res/progressBar/progressBar-4.png");
    p5 = QPixmap(":/res/progressBar/progressBar-5.png");
    p6 = QPixmap(":/res/progressBar/progressBar-6.png");
    p7 = QPixmap(":/res/progressBar/progressBar-7.png");
    p8 = QPixmap(":/res/progressBar/progressBar-8.png");
    p9 = QPixmap(":/res/progressBar/progressBar-9.png");
    p10 = QPixmap(":/res/progressBar/progressBar-10.png");
}

void ProgressBar::moveToCorner()
{
//    QPropertyAnimation *animation = new QPropertyAnimation(this, "pos", this);
//    animation->setStartValue(pos());
//    animation->setEndValue(scene()->views().first()->mapToScene(QPoint(68,90)));
//    animation->setDuration(530);
//    animation->setEasingCurve(QEasingCurve::InSine);
//    animation->start();
    QPoint p = QApplication::primaryScreen()->availableGeometry().topRight();
    int x = p.x()-115;
    setPos(scene()->views().first()->mapToScene(QPoint(x,70)));
}

void ProgressBar::setProgress(int level)
{
    if(level > 10) level = 10;
    if(level <  1) level =  1;
    progress_m = level;
    switch(level) {
    case  1:
        setPixmap(p1);
        break;
    case  2:
        setPixmap(p2);
        break;
    case  3:
        setPixmap(p3);
        break;
    case  4:
        setPixmap(p4);
        break;
    case  5:
        setPixmap(p5);
        break;
    case  6:
        setPixmap(p6);
        break;
    case  7:
        setPixmap(p7);
        break;
    case  8:
        setPixmap(p8);
        break;
    case  9:
        setPixmap(p9);
        break;
    case 10:
        setPixmap(p10);
        break;
    }
}
