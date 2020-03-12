#ifndef EDGESET_H
#define EDGESET_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QVector>
#include <utility>
#include <include/view/graph/node.h>

#define EDGE_Z_VALUE 377

//#define DEBUG

class EdgeSet : public QGraphicsItem
{
public:
    EdgeSet(QGraphicsItem *parent);

#ifdef DEBUG
    ~EdgeSet() override;
#else
    ~EdgeSet() = default;
#endif

    void addEdge(Node *n1, Node *n2);
    void addEdge(std::pair<Node *, Node *> edge) { addEdge(edge.first, edge.second); }
    void clear();
    int size();

public slots:
    void redraw();

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

private:
    QVector<QLineF> lines;
    std::vector<std::pair<Node *, Node *>> edges;
    std::vector<std::pair<uint, uint>> edgeByIds;

};

#endif // EDGESET_H
