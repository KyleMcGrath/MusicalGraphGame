#include "include/view/graph/edgeset.h"

#include <QGraphicsEffect>

static QBrush brush;
static QPen standardPen;
static QPen borderPen;
static QPen innerPen;

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

EdgeSet::EdgeSet(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
#ifdef DEBUG
        cerr << "Start constructing EdgeSet" << endl;
#endif
    setPos(0, 0);
    setZValue(EDGE_Z_VALUE);

    setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);

    standardPen.setWidth(60);
    standardPen.setCapStyle(Qt::RoundCap);

    innerPen.setWidth(50);
    innerPen.setCapStyle(Qt::RoundCap);
    innerPen.setColor("#F5F4FF");

    borderPen.setWidth(70);
    borderPen.setCapStyle(Qt::RoundCap);
    borderPen.setColor("#F5F5FF");

#ifdef DEBUG
        cerr << "Finish constructing EdgeSet" << endl;
#endif
}

#ifdef DEBUG
EdgeSet::~EdgeSet() {
        cerr << "Destroying EdgeSet" << endl;
}
#endif

void EdgeSet::addEdge(Node *n1, Node *n2)
{
#ifdef DEBUG
        cerr << "Start adding an edge" << endl;
#endif

    if(std::find(edgeByIds.cbegin(), edgeByIds.cend(), std::make_pair(n1->getId(), n2->getId())) == edgeByIds.cend()
            && std::find(edgeByIds.cbegin(), edgeByIds.cend(), std::make_pair(n2->getId(), n1->getId())) == edgeByIds.cend()) {
        QPointF n1Pos = this->mapFromScene(n1->scenePos());
        QPointF n2Pos = this->mapFromScene(n2->scenePos());

        double x1 = n1Pos.x()+n1->boundingRect().width()/2;
        double y1 = n1Pos.y()+n1->boundingRect().height()/2;
        double x2 = n2Pos.x()+n2->boundingRect().width()/2;
        double y2 = n2Pos.y()+n2->boundingRect().height()/2;

        lines.push_back(QLineF(x1, y1, x2, y2));

        edges.push_back(std::make_pair(n1, n2));

        edgeByIds.push_back(std::make_pair(n1->getId(), n2->getId()));
    }
#ifdef DEBUG
        cerr << "Finish adding an edge" << endl;
#endif
}

void EdgeSet::clear()
{
    lines.clear();
    edges.clear();
    edgeByIds.clear();
}

int EdgeSet::size()
{
    return lines.size();
}

void EdgeSet::redraw()
{
    lines.clear();
    for(auto it = edges.cbegin(); it != edges.cend(); it++) {
        Node *n1 = it->first;
        Node *n2 = it->second;

        QPointF n1Pos = this->mapFromScene(n1->scenePos());
        QPointF n2Pos = this->mapFromScene(n2->scenePos());


        double x1 = n1Pos.x()+n1->boundingRect().width()/2;
        double y1 = n1Pos.y()+n1->boundingRect().height()/2;
        double x2 = n2Pos.x()+n2->boundingRect().width()/2;
        double y2 = n2Pos.y()+n2->boundingRect().height()/2;

        lines.push_back(QLineF(x1, y1, x2, y2));
    }
}

QRectF EdgeSet::boundingRect() const
{
#ifdef DEBUG
        cerr << "In QRectF EdgeSet::boundingRect()" << endl;
#endif
    return QRectF(-10000, -10000, 100000, 100000);
}

void EdgeSet::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
#ifdef DEBUG
        cerr << "Start painting edge set" << endl;
#endif
    Q_UNUSED(option); Q_UNUSED(widget);
    redraw();

    painter->save();

    painter->setPen(borderPen);
    painter->drawLines(lines);

    painter->setPen(standardPen);
    painter->drawLines(lines);

    borderPen.setColor("#F5F5FF");
    painter->setPen(innerPen);
    painter->drawLines(lines);
    innerPen.setColor(QColor::fromRgb(30,30,200,200));
    innerPen.setWidth(50);
    painter->setPen(innerPen);
    painter->drawLines(lines);
    innerPen.setColor(QColor::fromRgb(30,50,200,200));
    innerPen.setWidth(40);
    painter->setPen(innerPen);
    painter->drawLines(lines);
    innerPen.setColor(QColor::fromRgb(50,30,200,200));
    innerPen.setWidth(30);
    painter->setPen(innerPen);
    painter->drawLines(lines);
    innerPen.setColor(QColor::fromRgb(50,60,255,200));
    innerPen.setWidth(20);
    painter->setPen(innerPen);
    painter->drawLines(lines);
    innerPen.setColor(QColor::fromRgb(50,140,255,200));
    innerPen.setWidth(10);
    painter->setPen(innerPen);
    painter->drawLines(lines);
    innerPen.setColor(QColor::fromRgb(50,190,255,200));
    innerPen.setWidth(5);
    painter->setPen(innerPen);
    painter->drawLines(lines);


    painter->restore();
#ifdef DEBUG
        cerr << "Finish painting edge set" << endl;
#endif
}
