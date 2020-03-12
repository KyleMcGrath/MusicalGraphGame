#include "include/view/main/summary/bar.h"

Bar::Bar(QGraphicsItem *parent, double moveS, double durS, double max) :
    QGraphicsItem(parent), moveS(moveS), durS(durS), max(max)
{
    if(moveS<0) moveS *= -1;
    if(durS<0) durS *= -1;
    pen.setWidth(2);
    pen.setColor(Qt::white);
    moveSBrush.setStyle(Qt::SolidPattern);
    durSBrush.setStyle(Qt::SolidPattern);
}

void Bar::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(load_m<100) load_m+=0.00000005*(100000000-load_m*load_m*load_m);

    double percent = load_m/100.0;

    double moveL = moveS*90;
    double durL = durS*90;

    if(max*max>0.01) {
        moveL /= 0.5*max;
        durL /= 0.5*max;
    }

    pen.setColor(Qt::black);
    painter->setPen(pen);
    painter->setBrush(moveSBrush);
    painter->drawRoundedRect(0, 0, percent*moveL, 50, 3, 3);
    painter->setBrush(durSBrush);
    painter->drawRoundedRect(percent*moveL, 0, percent*durL, 50, 3, 3);
    char *c = new char[9];
    snprintf(c, 9, "%2g", std::round(50.0*(moveS+durS))/100.0);

    pen.setColor(Qt::black);
    painter->setPen(pen);
    painter->drawText(QPointF(percent*(moveL+durL+2), 30), QString::fromStdString(c));

    delete []c;

    if(load_m>90 && legend) {
        painter->scale(0.333,0.333);
        painter->drawText(QPointF(5, 170), QString("moves"));
        painter->drawText(this->mapToItem(this, QPointF(std::max(3*(percent*moveL+5), 50.0), 170)), QString("duration"));
    }

    if(load_m<100) update();
}

QRectF Bar::boundingRect() const
{
    QRectF rect(-10000, -10000, 100000,100000);
    return rect;
}

double Bar::getDurS() const
{
    return durS;
}

void Bar::setDurS(double value)
{
    durS = value;
}

void Bar::setColor(COLOR color)
{
    if(color == COLOR::RED) {
        moveSBrush.setColor(QColor::fromRgb(255, 25, 30, 200));
        durSBrush.setColor(QColor::fromRgb(205, 25, 30, 100));
    } else if(color == COLOR::GREEN) {
        moveSBrush.setColor(QColor::fromRgb(25, 255, 30, 200));
        durSBrush.setColor(QColor::fromRgb(25, 205, 30, 100));
    } else if(color == COLOR::BLUE) {
        moveSBrush.setColor(QColor::fromRgb(30, 25, 255, 200));
        durSBrush.setColor(QColor::fromRgb(30, 25, 205, 100));
    }
}

double Bar::getMoveS() const
{
    return moveS;
}

void Bar::setMoveS(double value)
{
    moveS = value;
}
