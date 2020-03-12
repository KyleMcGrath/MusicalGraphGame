#include "include/view/main/scoreboard.h"

#include <iostream>

Scoreboard::Scoreboard(QGraphicsItem *parent) :
    QObject(nullptr),
    QGraphicsPixmapItem(parent)
{
    QPixmap p(tr(":/res/scoreboard.png"));
    p = p.transformed(QTransform::fromScale(1.3, 1.1));
    setPixmap(p);
    QGraphicsPixmapItem::setScale(0.175);

    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
    setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren);
    setFlag(QGraphicsItem::ItemContainsChildrenInShape);
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    setZValue(SCOREBOARD_ZVALUE);

    QFont font("Fruktur");

    targetValueTextItem = new QGraphicsTextItem(this);
    valueValueTextItem = new QGraphicsTextItem(this);

    targetValueTextItem->setDefaultTextColor(Qt::black);
    valueValueTextItem->setDefaultTextColor(Qt::black);

    targetValueTextItem->setFont(font);
    valueValueTextItem->setFont(font);

    targetValueTextItem->setScale(15);
    valueValueTextItem->setScale(15);

    QString targetString = tr(std::to_string(target_m).c_str());
    targetString.prepend("<div style='text-align:left'>  ");
    targetString.append("</div>");
    QString valueString = tr(std::to_string(value_m).c_str());
    valueString.prepend("<div style='text-align:left'>  ");
    valueString.append("</div>");

    targetValueTextItem->setHtml(targetString);
    valueValueTextItem->setHtml(valueString);

    valueValueTextItem->setPos(QPointF(-70,10));
    targetValueTextItem->setPos(QPointF(165,10));

    targetValueTextItem->setZValue(SCOREBOARD_ZVALUE);
    valueValueTextItem->setZValue(SCOREBOARD_ZVALUE);
}

QRectF Scoreboard::boundingRect() const
{
    return QGraphicsPixmapItem::boundingRect();
}

int Scoreboard::target() const
{
    return target_m;
}

void Scoreboard::setTarget(int value)
{
    target_m = value;
    QString targetString = tr(std::to_string(target_m).c_str());
    for(int i = targetString.length(); i <= 3; i++) targetString.prepend("&nbsp;");
    targetString.prepend("<div style='text-align:left'>  ");
    targetString.append("</div>");
    targetValueTextItem->setHtml(targetString);
    emit targetChanged();
}

int Scoreboard::value() const
{
    return value_m;
}

void Scoreboard::setValue(int value)
{
    value_m = value;
    QString valueString = tr(std::to_string(value_m).c_str());
    for(int i = valueString.length(); i <= 3; i++) valueString.prepend("&nbsp;");
    valueString.prepend("<div style='text-align:left'>  ");
    valueString.append("</div>");
    valueValueTextItem->setHtml(valueString);
    emit valueChanged();
}

void Scoreboard::setScale(double scale)
{
    scale_m = scale;
    QGraphicsPixmapItem::setScale(0.175*scale_m);
}

double Scoreboard::scale() const
{
    return scale_m;
}

int Scoreboard::label() const
{
    return label_m;
}

void Scoreboard::setLabel(int label)
{
    label_m = label;
}

void Scoreboard::labelAnimation()
{
    QPropertyAnimation *animationIn = new QPropertyAnimation(this, "label", this);
    animationIn->setStartValue(0);
    animationIn->setEndValue(100);
    animationIn->setEasingCurve(QEasingCurve::Linear);
    animationIn->setDuration(800);
    QPropertyAnimation *animationOut = new QPropertyAnimation(this, "label", this);
    animationOut->setStartValue(100);
    animationOut->setEndValue(0);
    animationOut->setEasingCurve(QEasingCurve::Linear);
    animationOut->setDuration(800);
    QObject::connect(animationIn, SIGNAL(finished()), animationOut, SLOT(start()));
    animationIn->start();
}

void Scoreboard::moveToCorner()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "pos", this);
    animation->setStartValue(pos());
    animation->setEndValue(scene()->views().first()->mapToScene(QPoint(48,30)));
    animation->setDuration(450);
    animation->setEasingCurve(QEasingCurve::InSine);
    animation->start();
}

void Scoreboard::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsPixmapItem::paint(painter, option, widget);
}

void Scoreboard::bounce()
{
    QPropertyAnimation *animationIn = new QPropertyAnimation(this, "scale", this);
    animationIn->setStartValue(1);
    animationIn->setEndValue(1.03);
    animationIn->setEasingCurve(QEasingCurve::InOutBounce);
    animationIn->setDuration(350);
    QPropertyAnimation *animationOut = new QPropertyAnimation(this, "scale", this);
    animationOut->setStartValue(1.03);
    animationOut->setEndValue(1);
    animationOut->setEasingCurve(QEasingCurve::InOutElastic);
    animationOut->setDuration(350);
    QObject::connect(animationIn, SIGNAL(finished()), animationOut, SLOT(start()));
    animationIn->start();
}
