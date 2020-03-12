#include "include/view/main/graphicsscene.h"

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

GraphicsScene::GraphicsScene(QObject *parent) :
    QGraphicsScene(parent)
{
    setPalette(QPalette(Qt::white));
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->setPos(event->scenePos());
    emit click(event->scenePos());
    QGraphicsScene::mousePressEvent(event);
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
#ifdef DEBUG
    cerr << "click " << mouseEvent->scenePos().x() << "x" << mouseEvent->scenePos().y() << endl;
#endif
    mouseEvent->setPos(mouseEvent->scenePos());
    emit click(mouseEvent->scenePos());
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    mouseEvent->setPos(mouseEvent->scenePos());
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}
