#ifndef CLOCK_H
#define CLOCK_H

#include <QObject>
#include <QApplication>
#include <QScreen>
#include <QGraphicsPixmapItem>
#include <QPropertyAnimation>
#include <QGraphicsView>

class Clock : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(int time READ time WRITE setTime NOTIFY timeChanged)
    Q_PROPERTY(int modulus READ modulus WRITE setModulus)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    explicit Clock(QGraphicsItem *parent = nullptr);

    int time() const;
    int modulus() const;
    void setRadianDelta();

signals:
    void timeChanged();

public slots:
    void setTime(int h);
    void timeTravelTo(int h);
    void timeTravel(int h);
    void setModulus(int mod);

    void moveToCorner();

private:
    int time_m = 0;
    int modulus_m = 12;
    double radianDelta;

    QPixmap m3;
    QPixmap m4;
    QPixmap m5;
    QPixmap m6;
    QPixmap m7;
    QPixmap m8;
    QPixmap m9;
    QPixmap m10;
    QPixmap m11;
    QPixmap m12;

    QGraphicsPixmapItem *hand;
};

#endif // CLOCK_H
