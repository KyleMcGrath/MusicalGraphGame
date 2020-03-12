#ifndef BAR_H
#define BAR_H

#include <QGraphicsItem>
#include <QPainter>
#include <cmath>


enum class COLOR {
    RED,
    BLUE,
    GREEN
};

class Bar : public QGraphicsItem
{
public:
    Bar(QGraphicsItem *parent = nullptr, double moveS = 1.0, double durS = 1.0, double max = 0.0);

    double getMoveS() const;
    void setMoveS(double value);

    double getDurS() const;
    void setDurS(double value);

    void setColor(COLOR color);

    void setLegend(bool value = true) { legend = value; }

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    double moveS = 1.0;
    double durS = 1.0;
    double load_m = 0;

    double max = 0;

    bool legend = false;

    QPen pen;
    QBrush moveSBrush;
    QBrush durSBrush;
};

#endif // BAR_H
