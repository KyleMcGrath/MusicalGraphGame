#include "include/view/main/menu/newmenuitem.h"

NewMenuItem::NewMenuItem(QObject *parent) : QObject(parent), QGraphicsPixmapItem(nullptr)
{
    setPixmap(QPixmap("://res/menu/NewMenuPixmap.png"));
    setZValue(NewMenuItemZValue);

    animation = new QPropertyAnimation(this, "scale", this);
    animation->setDuration(1700);
    animation->setStartValue(.3);
    animation->setEndValue(.32);
    animation->setEasingCurve(QEasingCurve::InBounce);
    animation->setDirection(QPropertyAnimation::Backward);

    connect(animation, SIGNAL(finished()), this, SLOT(reverseAnimation()));

    animation->start();

}

void NewMenuItem::reverseAnimation()
{
    if(animation->direction() == QPropertyAnimation::Backward)
        animation->setDirection(QPropertyAnimation::Forward);
    else
        animation->setDirection(QPropertyAnimation::Backward);
    animation->start();
}
