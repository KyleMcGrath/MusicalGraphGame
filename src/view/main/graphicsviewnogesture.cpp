#include "include/view/main/graphicsviewnogesture.h"

#ifdef DEBUG
using namespace std;
#include <iostream>
#endif

GraphicsViewNoGesture::GraphicsViewNoGesture(QWidget *parent) :
    QGraphicsView(parent)
{
    grabGesture(Qt::TapAndHoldGesture);
    setInteractive(false);
    setWindowFlag(Qt::WindowStaysOnTopHint);
}

bool GraphicsViewNoGesture::gestureEvent(QGestureEvent *event)
{

#ifdef DEBUG
        cerr << "bool GraphicsViewNoGesture::gestureEvent(QGestureEvent *event)" << endl;
#endif
    if(event->gesture(Qt::TapAndHoldGesture) != nullptr)
        emit displayMenu();
    else
        event->ignore();
}

void GraphicsViewNoGesture::setReceivingView(QGraphicsView *value)
{
    receivingView = value;
}

bool GraphicsViewNoGesture::event(QEvent *event)
{
    if(event->type() == QEvent::Gesture)
        gestureEvent(static_cast<QGestureEvent *>(event));
    QGraphicsView::event(event);
}

void GraphicsViewNoGesture::mouseReleaseEvent(QMouseEvent *event)
{

#ifdef DEBUG
        cerr << "void GraphicsViewNoGesture::mouseReleaseEvent(QMouseEvent *event)" << endl;
#endif
    QApplication::sendEvent(receivingView, event);
}
