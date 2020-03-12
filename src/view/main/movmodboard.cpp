#include "include/view/main/movmodboard.h"

#include <iostream>
using namespace std;

MovModBoard::MovModBoard(QGraphicsItem *parent) :
    QObject(nullptr),
    QGraphicsPixmapItem(parent)
{
    QPixmap p(tr(":/res/mod-mov.png"));
    p = p.transformed(QTransform::fromScale(1.3, 1.1));
    setPixmap(p);
    setScale(0.175);

    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
    setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren);
    setFlag(QGraphicsItem::ItemContainsChildrenInShape);
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    setZValue(MOVMODBOARD_ZVALUE);

    QFont font("Fruktur");

    problemIndexTextItem = new QGraphicsTextItem(this);
    modulusValueTextItem = new QGraphicsTextItem(this);
    movesValueTextItem = new QGraphicsTextItem(this);

    problemIndexTextItem->setDefaultTextColor(Qt::black);
    modulusValueTextItem->setDefaultTextColor(Qt::black);
    movesValueTextItem->setDefaultTextColor(Qt::black);

    problemIndexTextItem->setFont(font);
    modulusValueTextItem->setFont(font);
    movesValueTextItem->setFont(font);

    problemIndexTextItem->setScale(13);
    modulusValueTextItem->setScale(13);
    movesValueTextItem->setScale(13);

    QSettings settings(tr("MusicalGraph"), tr("MusicalGraph"), nullptr);
    QString gameVersion;
    if(settings.contains(tr("gameVersion")))
        gameVersion = settings.value(tr("gameVersion")).toString();
    else gameVersion = tr("baby");
    if(settings.contains(gameVersion+tr("problemIndex")))
        problemIndex_m = settings.value(gameVersion+tr("problemIndex")).toInt();
    else problemIndex_m = 0;

    QString problemIndexString = tr(std::to_string(problemIndex_m).c_str());
    problemIndexString.prepend("<div style='text-align:left'>  ");
    problemIndexString.append("</div>");
    QString modString = tr(std::to_string(modulus_m).c_str());
    modString.prepend("<div style='text-align:left'>  ");
    modString.append("</div>");
    QString movesString = tr(std::to_string(moves_m).c_str());
    movesString.prepend("<div style='text-align:left'>  ");
    movesString.append("</div>");

    problemIndexTextItem->setHtml(problemIndexString);
    modulusValueTextItem->setHtml(modString);
    movesValueTextItem->setHtml(movesString);

    problemIndexTextItem->setPos(QPointF(-70,-290));
    modulusValueTextItem->setPos(QPointF(-70,10));
    movesValueTextItem->setPos(QPointF(160,10));

    problemIndexTextItem->setZValue(MOVMODBOARD_ZVALUE);
    modulusValueTextItem->setZValue(MOVMODBOARD_ZVALUE);
    movesValueTextItem->setZValue(MOVMODBOARD_ZVALUE);

    problemIndexTextItem->setOpacity(0.075);

}

int MovModBoard::problemIndex() const
{
    return problemIndex_m;
}

void MovModBoard::setProblemIndex(int problemIndex)
{
    problemIndex_m = problemIndex;
    QString problemIndexString = tr(std::to_string(problemIndex_m).c_str());
    problemIndexString.prepend("<div style='text-align:left'>  ");
    problemIndexString.append("</div>");
    problemIndexTextItem->setHtml(problemIndexString);
    emit problemIndexChanged();
}


int MovModBoard::modulus() const
{
    return modulus_m;
}

void MovModBoard::setModulus(int value)
{
    modulus_m = value;
    QString modString = tr(std::to_string(modulus_m).c_str());
    for(int i = modString.length(); i <= 3; i++) modString.prepend("&nbsp;");
    modString.prepend("<div style='text-align:left'>  ");
    modString.append("</div>");
    modulusValueTextItem->setHtml(modString);
    emit modulusChanged();
}

int MovModBoard::moves() const
{
    return moves_m;
}

void MovModBoard::setMoves(int value)
{
    moves_m = value;
    QString movesString = tr(std::to_string(moves_m).c_str());
    for(int i = movesString.length(); i <= 3; i++) movesString.prepend("&nbsp;");
    movesString.prepend("<div style='text-align:left'>  ");
    movesString.append("</div>");
    movesValueTextItem->setHtml(movesString);
    emit movesChanged();
}

void MovModBoard::moveToCorner()
{
    QPoint p = QApplication::primaryScreen()->availableGeometry().bottomLeft();
    QPropertyAnimation *animation = new QPropertyAnimation(this, "pos", this);
    animation->setStartValue(pos());
    animation->setEndValue(scene()->views().first()->mapToScene(48,p.y()-100));
    animation->setDuration(390);
    animation->setEasingCurve(QEasingCurve::InSine);
    animation->start();
}
