#ifndef MULTIPLAYERMENUITEM_H
#define MULTIPLAYERMENUITEM_H

#include <QGraphicsPixmapItem>
#include <QPropertyAnimation>

#define MultiPlayerMenuItemZValue 20002

class MultiPlayerMenuItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(double scale READ scale WRITE setScale)
public:
    explicit MultiPlayerMenuItem(QObject *parent = nullptr);

signals:

public slots:

private:
    QPropertyAnimation *animation;

private slots:
    void reverseAnimation();
};

#endif // MULTIPLAYERMENUITEM_H
