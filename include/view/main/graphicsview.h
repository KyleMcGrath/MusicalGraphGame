#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QDesktopWidget>
#include <QGraphicsView>
#include <QGestureEvent>
#include <QSwipeGesture>
#include <QPinchGesture>
//#include <QPanGesture>
#include <QTapAndHoldGesture>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <cmath>
#include <QApplication>
#include "include/view/main/scoreboard.h"

#ifdef Q_OS_IOS
#include "AudioToolbox/AudioServices.h"
#endif

//#define DEBUG

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
    Q_PROPERTY(double scale READ getScaleFactor WRITE setScaleFactor NOTIFY scaleChanged)
public:
    GraphicsView(QWidget *parent);

    void setRotation(double value);

    void setScaleFactor(double value);

    double getScaleFactor() const;

    double getRotation() const;


    void setScoreboard(Scoreboard *value);

signals:
    void scaleChanged();
    void redraw();
    void move();
    void showMenu();

protected:
    bool viewportEvent(QEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event);
    bool event(QEvent *event);

private:
    double rotation = 0;
    double scale_m = 1;
    bool labelScoreboard = false;

    bool gestureEvent(QGestureEvent *gesture);
    bool tapHoldEvent(QTapAndHoldGesture *gesture);

    Scoreboard *scoreboard;
};

#endif // GRAPHICSVIEW_H
