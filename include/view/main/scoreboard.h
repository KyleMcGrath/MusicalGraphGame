#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QPainter>
#include <QGraphicsTextItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <cmath>
#include <QPropertyAnimation>
#include <QSettings>

#define SCOREBOARD_ZVALUE 10000

class Scoreboard : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(double scale READ scale WRITE setScale)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(int label READ label WRITE setLabel)

public:
    Scoreboard(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;

    void bounce();

public slots:
    int target() const;
    void setTarget(int value);

    int value() const;
    void setValue(int value);

    void setScale(double scale);
    double scale() const;

    int label() const;
    void setLabel(int label = true);

    void labelAnimation();

    void moveToCorner();

signals:
    void targetChanged();
    void valueChanged();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

private:
    int target_m = 0;
    int value_m = 0;
    double scale_m = 1;
    int label_m = false;

    QGraphicsTextItem *targetValueTextItem;
    QGraphicsTextItem *valueValueTextItem;

};

#endif // SCOREBOARD_H
