#ifndef SBOARD_H
#define SBOARD_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QPropertyAnimation>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QApplication>
#include <QScreen>

class SBoard : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(int score READ score WRITE setScore NOTIFY scoreChanged)
public:
    SBoard(QGraphicsItem *parent = nullptr);

    double score() const;
    void setScore(double value);

public slots:
    void moveToCorner();

signals:
    void scoreChanged();

private:
    double score_m = 1;
    QGraphicsTextItem *textItem;
};

#endif // SBOARD_H
