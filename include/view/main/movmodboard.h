#ifndef MOVMODBOARD_H
#define MOVMODBOARD_H

#include <QApplication>
#include <QScreen>
#include <QObject>
#include <QGraphicsPixmapItem>
#include <QFont>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPropertyAnimation>
#include <QSettings>

#define MOVMODBOARD_ZVALUE 10010

class MovModBoard : public QObject, public QGraphicsPixmapItem
{
    Q_PROPERTY(int problemIndex READ problemIndex WRITE setProblemIndex NOTIFY problemIndexChanged)
    Q_PROPERTY(int modulus READ modulus WRITE setModulus NOTIFY modulusChanged)
    Q_PROPERTY(int moves READ moves WRITE setMoves NOTIFY movesChanged)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

    Q_OBJECT
public:
    explicit MovModBoard(QGraphicsItem *parent = nullptr);

    int problemIndex() const;
    void setProblemIndex(int problemIndex);

    int modulus() const;
    void setModulus(int value);

    int moves() const;
    void setMoves(int value);

signals:
    void problemIndexChanged();
    void modulusChanged();
    void movesChanged();

public slots:
    void moveToCorner();

private:
    int modulus_m = 0;
    int moves_m = 0;
    int problemIndex_m = 0;

    QGraphicsTextItem *problemIndexTextItem;
    QGraphicsTextItem *modulusValueTextItem;
    QGraphicsTextItem *movesValueTextItem;

};

#endif // MOVMODBOARD_H
