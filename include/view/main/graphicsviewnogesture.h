#ifndef GRAPHICSVIEWNOGESTURE_H
#define GRAPHICSVIEWNOGESTURE_H

#include <QGraphicsView>
#include <QGestureEvent>
#include <QMouseEvent>
#include <QApplication>
#include "include/view/main/graphicsview.h"

//#define DEBUG

class GraphicsViewNoGesture : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsViewNoGesture(QWidget *parent);
    ~GraphicsViewNoGesture() = default;

    void setReceivingView(QGraphicsView *value);

signals:
    void displayMenu();

protected:
    bool event(QEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event);

private:
    bool gestureEvent(QGestureEvent *gesture);
    QGraphicsView *receivingView;
};

#endif // GRAPHICSVIEWNOGESTURE_H
