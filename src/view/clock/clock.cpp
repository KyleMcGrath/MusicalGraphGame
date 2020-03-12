#include "include/view/clock/clock.h"

Clock::Clock(QGraphicsItem *parent) : QObject(nullptr), QGraphicsPixmapItem(parent)
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
    setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren);
    setFlag(QGraphicsItem::ItemContainsChildrenInShape);
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    m3 = QPixmap(":/res/clock/clockFace-3.png");
    m4 = QPixmap(":/res/clock/clockFace-4.png");
    m5 = QPixmap(":/res/clock/clockFace-5.png");
    m6 = QPixmap(":/res/clock/clockFace-6.png");
    m7 = QPixmap(":/res/clock/clockFace-7.png");
    m8 = QPixmap(":/res/clock/clockFace-8.png");
    m9 = QPixmap(":/res/clock/clockFace-9.png");
    m10 = QPixmap(":/res/clock/clockFace-10.png");
    m11 = QPixmap(":/res/clock/clockFace-11.png");
    m12 = QPixmap(":/res/clock/clockFace-12.png");
    hand = new QGraphicsPixmapItem(QPixmap(":/res/clock/clockHand.png"), this);
    hand->setPos(0,0);
    hand->setZValue(50000);

    setModulus(12);
    setRadianDelta();
    setTime(0);

    setScale(0.175);

    hand->setTransformOriginPoint(hand->boundingRect().center());
}

int Clock::time() const
{
    return time_m;
}

int Clock::modulus() const
{
    return modulus_m;
}

void Clock::setRadianDelta()
{
    switch(modulus_m) {
    case 3: radianDelta = 120; break;
    case 4: radianDelta = 90; break;
    case 5: radianDelta = 72; break;
    case 6: radianDelta = 60; break;
    case 7: radianDelta = 51.4285714285714; break;
    case 8: radianDelta = 45; break;
    case 9: radianDelta = 40; break;
    case 10: radianDelta = 36; break;
    case 11: radianDelta = 32.7272727272727; break;
    case 12: radianDelta = 30; break;
    }
}

void Clock::setTime(int h)
{
    h%=2*modulus_m;
    time_m = h;
    hand->setRotation(radianDelta*(h%modulus_m));
}

void Clock::timeTravelTo(int h)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "time", this);
    animation->setStartValue(time_m);
    animation->setEndValue(h);
    animation->setDuration(275*abs(h-time_m));
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->start();
}

void Clock::timeTravel(int h)
{
    timeTravelTo(time_m+h);
}

void Clock::setModulus(int mod)
{
    if(mod == modulus_m) return;
    modulus_m = mod;
    int effectiveModulus = modulus_m;
    if(modulus_m<24) effectiveModulus %= 12;
    switch(effectiveModulus) {
    case 3: setPixmap(m3); break;
    case 4: setPixmap(m4); break;
    case 5: setPixmap(m5); break;
    case 6: setPixmap(m6); break;
    case 7: setPixmap(m7); break;
    case 8: setPixmap(m8); break;
    case 9: setPixmap(m9); break;
    case 10: setPixmap(m10); break;
    case 11: setPixmap(m11); break;
    case 12: setPixmap(m12); break;
    }
    setTime(time_m);
}

void Clock::moveToCorner()
{
    QPoint p = QApplication::primaryScreen()->availableGeometry().bottomLeft();
    QPropertyAnimation *animation = new QPropertyAnimation(this, "pos", this);
    animation->setStartValue(pos());
    animation->setEndValue(scene()->views().first()->mapToScene(40,p.y()-260));
    animation->setDuration(390);
    animation->setEasingCurve(QEasingCurve::InSine);
    animation->start();
}
