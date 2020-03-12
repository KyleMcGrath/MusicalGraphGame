#include "include/view/main/menu/multiplayermenuitem.h"

MultiPlayerMenuItem::MultiPlayerMenuItem(QObject *parent) : QObject(parent), QGraphicsPixmapItem(nullptr)
{
    setPixmap(QPixmap("://res/menu/MultiPlayerMenuPixmap.png"));
    setZValue(MultiPlayerMenuItemZValue);

    animation = new QPropertyAnimation(this, "scale", this);
    animation->setDuration(1600);
    animation->setStartValue(.3);
    animation->setEndValue(.32);
    animation->setEasingCurve(QEasingCurve::InBounce);

    connect(animation, SIGNAL(finished()), this, SLOT(reverseAnimation()));

    animation->start();

}

void MultiPlayerMenuItem::reverseAnimation()
{
    if(animation->direction() == QPropertyAnimation::Backward)
        animation->setDirection(QPropertyAnimation::Forward);
    else
        animation->setDirection(QPropertyAnimation::Backward);
    animation->start();
}
