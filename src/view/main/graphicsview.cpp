#include "include/view/main/graphicsview.h"

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

#define PI 3.141592
#define DEFAULT_SCALE 0.0625

GraphicsView::GraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    grabGesture(Qt::TapAndHoldGesture);
    setScaleFactor(1);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    setSceneRect(QRectF(-500000, -500000, 1000000, 1000000));
    setCacheMode(QGraphicsView::CacheBackground);
    setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);
    setAttribute(Qt::WA_AcceptTouchEvents);
    setPalette(QPalette(Qt::white));

    int labelCount = 10;
    QSettings settings(tr("MusicalGraph"), tr("MusicalGraph"), this);
    if(settings.contains(tr("scoreboardLabel")))
        labelCount = settings.value(tr("scoreboardLabel")).toInt();

    if(labelCount<1) labelScoreboard = false;
    else labelScoreboard = true;

    settings.setValue(tr("scoreboardLabel"), --labelCount);

}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    emit move();
    if(!scoreboard) return;
    QGraphicsItem *item = scene()->itemAt(this->mapToScene(event->pos()), viewportTransform());
    if(!item) return;

    if(abs(item->zValue()-SCOREBOARD_ZVALUE)<=1) {
        scoreboard->bounce();
        if(labelScoreboard)
            scoreboard->labelAnimation();
    }
    QGraphicsView::mouseReleaseEvent(event);
}

bool GraphicsView::event(QEvent *event)
{
    if(event->type() == QEvent::Gesture)
        gestureEvent(static_cast<QGestureEvent *>(event));
    if(event->type() == QEvent::MouseButtonRelease)
        mouseReleaseEvent(static_cast<QMouseEvent *>(event));
    return QGraphicsView::event(event);
}

double GraphicsView::getRotation() const
{
    return rotation;
}

double GraphicsView::getScaleFactor() const
{
    return scale_m;
}

void GraphicsView::setScaleFactor(double value)
{
    emit move();
    scale_m = value;
    setTransform(QTransform::fromScale(DEFAULT_SCALE*scale_m, DEFAULT_SCALE*scale_m));
}

void GraphicsView::setRotation(double value)
{
    emit move();
    rotation = value;
    rotation = fmod(rotation, 360.0);
}

bool GraphicsView::gestureEvent(QGestureEvent *event)
{

    if(QGesture *tapHold = event->gesture(Qt::TapAndHoldGesture))
        tapHoldEvent(static_cast<QTapAndHoldGesture *>(tapHold));
    emit redraw();
    return true;
}

bool GraphicsView::tapHoldEvent(QTapAndHoldGesture *gesture)
{
    if(!scoreboard) return false;
    QGraphicsItem *item = scene()->itemAt(this->mapToScene(gesture->position().toPoint()), viewportTransform());
    if(!item) return false;
    setDragMode(QGraphicsView::NoDrag);
    if(abs(item->zValue()-SCOREBOARD_ZVALUE)<=1) {
#ifdef Q_OS_IOS
    AudioServicesPlayAlertSound(kSystemSoundID_Vibrate);
#endif
        emit showMenu();
    }
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void GraphicsView::setScoreboard(Scoreboard *value)
{
    scoreboard = value;
}

bool GraphicsView::viewportEvent(QEvent *event)
{
    switch(event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
        QList<QTouchEvent::TouchPoint> points = touchEvent->touchPoints();
        if(points.size() == 2) {
            double d1 = QLineF(points.first().pos(), points.last().pos()).length();
            double d2 = QLineF(points.first().lastPos(), points.last().lastPos()).length();

            setScaleFactor(std::min(std::max(scale_m*d1*d1/(d2*d2)*sqrt(d1/(d1+1)), 0.25), 2.0));
            return true;
        }

    //default: break;
    }

    return QGraphicsView::viewportEvent(event);
}
