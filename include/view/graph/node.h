#ifndef NODE_H
#define NODE_H

#include <QStyleOptionGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QPropertyAnimation>
#include <QObject>
#include <QPixmap>
#include <vector>
#include <QPainter>
#include <random>
#include <chrono>
#include <QFontMetrics>
#include <cmath>
#include <QPixmapCache>
#include <QMutex>

//#define DEBUG


enum class status {
    ACTIVE, INACTIVE
};

#define NODE_Z_VALUE 993

class Node : public QObject, public QGraphicsPixmapItem
{

    Q_OBJECT

    Q_PROPERTY(double radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos NOTIFY posChanged)
    Q_PROPERTY(double alpha READ alpha WRITE setAlpha NOTIFY alphaChanged)
    Q_PROPERTY(double redglow READ redglow WRITE setRedglow)
    Q_PROPERTY(double aspectRatio READ aspectRatio WRITE setAspectRatio)

public:
    Node(QGraphicsItem *parent = nullptr);
    Node(QGraphicsItem *parent, int x, int y, int value_m);
    ~Node();

    void activate();
    void inactivate();

    bool connect(Node *);
    bool isConnected(Node *);

    ::status getS() const;

    uint getId() const;
    void setId(const uint &value);

    int value() const;
    void setValue(int value_m);

    double radius() const;
    void setRadius(double value_m);

    double alpha() const;
    void setAlpha(double value);

    QPointF pos() const;
    void setPos(const QPointF &value);

    void setPixmapFromValue();

    std::vector<Node *> getNeighbors() const;

    static void resetCounter();

    void setAlwaysDrawNumbers(bool enabled = true);

    void moveFromTo(QPointF from, QPointF to);

    bool getSelectable() const;
    void setSelectable(bool value);

    bool redglow() const { return redglow_m; }
    void setRedglow(bool value = true) { redglow_m = value; }

    void setValueTrap(int modulus = 12);

    double aspectRatio() const;
    void setAspectRatio(double value);

    void squish();

signals:
    void radiusChanged();
    void valueChanged();
    void alphaChanged();
    void posChanged();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

private:
    uint id = 0;
    ::status s;
    std::vector<Node *> neighbors;

    QPointF pos_m;
    double radius_m;
    double alpha_m;
    int value_m;
    bool alwaysDrawNumbers = false;
    bool selectable = true;

    double theta = 0;
    bool redglow_m = false;
    double aspectRatio_m = 1;
    int squishIndex = 0;
    QPropertyAnimation *squish1;
    QPropertyAnimation *squish2;
    QPropertyAnimation *valueAnimation;

    std::vector<QPixmap *> pixmaps;
    int originalValue = 0;
    bool trapMode = false;

};

#endif // NODE_H
