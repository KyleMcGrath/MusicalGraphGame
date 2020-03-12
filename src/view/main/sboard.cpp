#include "include/view/main/sboard.h"

SBoard::SBoard(QGraphicsItem *parent) :
    QObject(nullptr),
    QGraphicsPixmapItem(parent)
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
    setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren);
    setFlag(QGraphicsItem::ItemContainsChildrenInShape);
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    setZValue(10000);

    QPixmap p = QPixmap("://res/sBoard.png");
    p = p.transformed(QTransform::fromScale(1.3, 1.1));
    setPixmap(p);
    setScale(0.175);

    setPixmap(p);
    textItem = new QGraphicsTextItem(this);
    textItem->setDefaultTextColor(Qt::black);
    QFont font("Fruktur");

    textItem->setFont(font);
    textItem->setPlainText("1.00");
    textItem->setScale(15);
    textItem->setPos(175,-30);
}

void SBoard::moveToCorner()
{
    QPoint p = QApplication::primaryScreen()->availableGeometry().topRight();
    QPropertyAnimation *animation = new QPropertyAnimation(this, "pos", this);
    animation->setStartValue(pos());
    animation->setEndValue(scene()->views().first()->mapToScene(QPoint(p.x()-50,30)));
    animation->setDuration(575);
    animation->setEasingCurve(QEasingCurve::InSine);
    animation->start();
    setPos(scene()->views().first()->mapToScene(QPoint(p.x()-150,4)));
}

double SBoard::score() const
{
    return score_m;
}

void SBoard::setScore(double value)
{
    char *n = new char[4];
    sprintf(n, "%4g", value);
    textItem->setPlainText(n);
    score_m = value;
    delete[] n;
}
