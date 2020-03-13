#include "include/view/graph/node.h"

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

static uint ID_COUNTER = 0;
static QMutex mu;

Node::Node(QGraphicsItem *parent) : QObject(nullptr), QGraphicsPixmapItem(parent)
{
#ifdef DEBUG
    cerr << "  Construct Node" << endl;
#endif
    setZValue(NODE_Z_VALUE);
    std::mt19937_64 gen;
    gen.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(0, 11);
    value_m = dist(gen);
    originalValue = value_m;
    mu.lock();
    id = ++ID_COUNTER;
    mu.unlock();
    inactivate();
    setPixmapFromValue();
    radius_m = 1;
    setTransformOriginPoint(boundingRect().center());
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);

    squish1 = new QPropertyAnimation(this, "aspectRatio", this);
    squish1->setEasingCurve(QEasingCurve::InOutBounce);
    squish2 = new QPropertyAnimation(this, "aspectRatio", this);
    squish2->setEasingCurve(QEasingCurve::InOutBounce);

    valueAnimation = new QPropertyAnimation(this, "value", this);
    valueAnimation->setEasingCurve(QEasingCurve::Linear);

#ifdef DEBUG
    cerr << "  Finish constructing Node " << to_string(id) << endl;
#endif
}

Node::Node(QGraphicsItem *parent, int x, int y, int value) : Node(parent)
{
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
    setZValue(NODE_Z_VALUE);
    setValue(value);
    setPos(QPointF(x, y));
    inactivate();
    setPixmapFromValue();
}

Node::~Node()
{
    for(auto it = pixmaps.cbegin(); it != pixmaps.cend(); it++)
        delete *it;
}

void Node::activate()
{
#ifdef DEBUG
    cerr << "  Activate Node " << to_string(id) << endl;
#endif
    if(s != ::status::ACTIVE) {
        s = ::status::ACTIVE;
        setPixmapFromValue();
        setSelectable(false);
    }
}

void Node::inactivate()
{
#ifdef DEBUG
    cerr << "  Inactivate Node " << to_string(id) << endl;
#endif
    if(s != ::status::INACTIVE) {
        s = ::status::INACTIVE;
        setPixmapFromValue();
        setSelectable(true);
    }
}

bool Node::connect(Node *n)
{
#ifdef DEBUG
    cerr << " In bool Node::connect(Node *)" << endl;
    cerr << "Connect  this Node (" << to_string(id) << ") to Node " << to_string(n->getId()) << endl;
#endif
    if(n != nullptr && !isConnected(n))
        neighbors.push_back(n);
    else return false;
    return true;
}

bool Node::isConnected(Node *n)
{
#ifdef DEBUG
    cerr << " In bool Node::isConnected(Node *n)" << endl;
#endif
    for(auto it = neighbors.cbegin(); it != neighbors.cend(); it++)
        if((*it!=nullptr) && (*it)->getId() == n->getId())
            return true;
    return false;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // TODO: Find a non-hacky way to do this
    ((QStyleOptionGraphicsItem *) option)->state.setFlag(QStyle::State_Selected, false);
    QGraphicsPixmapItem::paint(painter, option, widget);
    if(redglow_m) {
        theta += 0.1;
        theta = fmod(theta, 1);
        QPen pen;
        pen.setWidth(200);
        pen.setColor(QColor::fromRgb(20,45,255,theta*40+10));
        painter->setPen(pen);
        painter->drawEllipse(theta*boundingRect().width()/2+(1-theta)*110,theta*boundingRect().height()/2+(1-theta)*110, (1-theta)*boundingRect().width()/2, (1-theta)*boundingRect().height()/2);
        pen.setColor(QColor::fromRgb(245,35,5,theta*40+10));
        painter->setPen(pen);
        pen.setWidth(210);
        painter->drawEllipse(theta*boundingRect().width()/2+(1-theta)*110,theta*boundingRect().height()/2+(1-theta)*110, (1-theta)*boundingRect().width()/1.7, (1-theta)*boundingRect().height()/1.9);
        pen.setColor(QColor::fromRgb(255,55,45,theta*40+5));
        painter->setPen(pen);
        painter->drawEllipse(theta*boundingRect().width()/2+(1-theta)*110,theta*boundingRect().height()/2+(1-theta)*110, (1-theta)*boundingRect().width()/1.8, (1-theta)*boundingRect().height()/1.7);
        pen.setColor(QColor::fromRgb(255,15,45,theta*40+5));
        painter->setPen(pen);
        painter->drawEllipse(theta*boundingRect().width()/2+(1-theta)*110,theta*boundingRect().height()/2+(1-theta)*110, (1-theta)*boundingRect().width()/2.1, (1-theta)*boundingRect().height()/1.6);

        pen.setColor(QColor::fromRgb(255,5,45,theta*40+15));
        painter->setPen(pen);
        painter->drawEllipse(theta*boundingRect().width()/2+(1-theta)*110,theta*boundingRect().height()/2+(1-theta)*110, (1-theta)*boundingRect().width()/1.9, (1-theta)*boundingRect().height()/2.0);

        pen.setColor(QColor::fromRgb(20,45,255,200));
        pen.setWidth(2);
        painter->setPen(pen);
        painter->scale(10+theta,10+theta);
        if(theta<0.5)
            painter->setOpacity(2*theta);
        if(theta>0.5)
            painter->setOpacity(2-2*theta);
        painter->setFont(QFont("Fruktur"));
        painter->drawText(QPointF(-50,0), tr("Press Me"));

    }
}

double Node::aspectRatio() const
{
    return aspectRatio_m;
}

void Node::setAspectRatio(double value)
{
    aspectRatio_m = value;
    QTransform t;
    t.scale(aspectRatio_m, 1.0/aspectRatio_m);
    setTransform(t);
}

void Node::squish()
{
    ++squishIndex;
    squishIndex%=3;
    if(squishIndex == 0) return;
    if(squishIndex+1%2==0) {
        squish1->setStartValue(1);
        squish1->setEndValue(1.05);
        squish1->setDuration(10);

        squish2->setStartValue(1.05);
        squish2->setEndValue(.95);
        squish2->setDuration(10);
        QObject::connect(squish1, SIGNAL(finished()), squish2, SLOT(start()));
        squish1->start();
    } else {
        squish1->setStartValue(.95);
        squish1->setEndValue(1.05);
        squish1->setDuration(10);

        squish2->setStartValue(1.05);
        squish2->setEndValue(1);
        squish2->setDuration(10);
        QObject::connect(squish1, SIGNAL(finished()), squish2, SLOT(start()));
        squish1->start();
    }
}

bool Node::getSelectable() const
{
    return selectable;
}

void Node::setSelectable(bool value)
{
    selectable = value;
}

void Node::setValueTrap(int modulus)
{
    if(modulus == 0) {
        valueAnimation->stop();
        value_m = originalValue;
        return;
    }
    originalValue = value_m;

    for(int i = 0; i < modulus; i++) {
        setValue(i);
        setPixmapFromValue();
        QPixmap p = pixmap();
        QPixmap *p_ptr = new QPixmap(p);
        pixmaps.push_back(p_ptr);
    }

    modulus = std::abs(modulus);
    valueAnimation->setStartValue(0);
    valueAnimation->setEndValue(modulus);
    valueAnimation->setDuration(75*modulus);
    QObject::connect(valueAnimation, SIGNAL(finished()), valueAnimation, SLOT(start()));
    valueAnimation->start();
}

void Node::setId(const uint &value)
{
    id = value;
}

std::vector<Node *> Node::getNeighbors() const
{
    return neighbors;
}

void Node::resetCounter()
{
    mu.lock();
    ID_COUNTER = 0;
    mu.unlock();
}

void Node::setAlwaysDrawNumbers(bool enabled)
{
    alwaysDrawNumbers = enabled;
}

void Node::moveFromTo(QPointF from, QPointF to)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "pos", this);
    animation->setStartValue(QPointF(from));
    animation->setEndValue(QPointF(to));
    animation->setDuration(1000);
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->start();
}

QPointF Node::pos() const
{
    return pos_m;
}

void Node::setPos(const QPointF &value)
{
    QGraphicsItem::setPos(value);
    pos_m = value;
}

double Node::alpha() const
{
    return alpha_m;
}

void Node::setAlpha(double value)
{
    setOpacity(std::min(value/255.0, 1.0));
    alpha_m = value;
}

double Node::radius() const
{
    return radius_m;
}

void Node::setRadius(double value)
{
    if(abs(radius_m-value)>0.01) emit radiusChanged();
    double factor=value/radius_m;
    setScale(factor);
    radius_m = value;
}

void Node::setPixmapFromValue()
{
    QPixmap pixmap;
    if(s == status::INACTIVE) {
        if(!QPixmapCache::find(tr("blank"), &pixmap)) {
#ifdef DEBUG
            cerr << "node_11_blank not in cache" << endl;
#endif
            pixmap = QPixmap(QObject::tr(":/res/nodes/blank/node.png"));
            QPixmapCache::insert(tr("blank"), pixmap);
        }
    } else
        if(!QPixmapCache::find(tr("node_0_active"), &pixmap)) {
#ifdef DEBUG
            cerr << "node_0_active not in cache" << endl;
#endif
            pixmap = QPixmap(QObject::tr(":/res/active nodes/activenode.png"));
            QPixmapCache::insert(tr("node_0_active"), pixmap);
        }

    QPainter *painter = new QPainter(&pixmap);

    QBrush b;
    QPen p;
    p.setBrush(b);
    p.setWidth(70);
    painter->setBrush(b);
    p.setColor(Qt::black); b.setColor(Qt::black);
    painter->setPen(p);
    QString valueString = QString::fromStdString(std::to_string(value_m));
    if(value_m>9) {
        painter->scale(17, 17);
        painter->drawText(QPointF(5, 20), valueString);
    } else {
        painter->scale(23, 23);
        painter->drawText(QPointF(5, 17), valueString);
    }
    delete painter;

    setPixmap(pixmap);
}

void Node::setValue(int value)
{
    if(trapMode) {
        int index = value % pixmaps.size();
        setPixmap(*pixmaps.at(index));
    } else {
        this->value_m = value;
        originalValue = value_m;
        setPixmapFromValue();
    }
}

int Node::value() const
{
#ifdef DEBUG
    cerr << " In int Node::getValue() const" << endl;
#endif
    return value_m;
}

uint Node::getId() const
{
#ifdef DEBUG
    cerr << " In int Node::getId() const " << to_string(id) << endl;
#endif
    return id;
}

::status Node::getS() const
{
#ifdef DEBUG
    cerr << " In ::status Node::getS() const" << endl;
#endif
    return s;
}
