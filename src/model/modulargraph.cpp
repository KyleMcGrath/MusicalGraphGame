#include "include/model/modulargraph.h"
#include "ui_modulargraph.h"

#include "include/model/embeddings.h"

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

enum class GAMETYPE {
    SINGLEPLAYER,
    MULTIPLAYER
};

static constexpr double x_shift = 500;
static constexpr double y_shift = 500;
static constexpr double edge_scalingFactor = .8;

class ModularGraph::Impl {

    typedef std::vector<Node *>::size_type size;

public:
    ModularGraph *parent;
    std::vector<Node *> nodes;
    EdgeSet *edgeSet;
    Node *activeNode;
    Node *nextNodePtr;
    Ui::ModularGraph *ui;
    int value;
    int target;
    int moves;
    int modulus = 12;
    std::uniform_int_distribution<int> valueDist;
    std::mt19937_64 gen;
    std::vector<int> melody;
    double scale = 1;
    Node *parentNode = new Node();
    QPropertyAnimation *activeNodeDanceForward;
    QPropertyAnimation *activeNodeDanceBackward;
    int problemIndex = 3;
    bool scaleSet = false;
    std::chrono::time_point<std::chrono::steady_clock> start;
    bool sound = true;
    QString gameVersion;
    QJsonObject gameplay;
    QJsonArray moveList;
    int requiredDepth = 0;
    int cumulative = 0;
    GAMETYPE gametype = GAMETYPE::SINGLEPLAYER;
    int targetCounter = 0;
    int systemIndex = 0;
    std::vector<QGraphicsItem *> itemBuffer;

    Impl(ModularGraph *parent) {
#ifdef DEBUG
        cerr << "Construct Impl" << endl;
#endif
        this->parent = parent;


#ifdef DEBUG
        cerr << "Finish constructing Impl" << endl;
#endif
    }

    ~Impl() {
#ifdef DEBUG
        cerr << "Destroy Impl" << endl;
#endif
        delete parentNode; delete activeNodeDanceForward; delete activeNodeDanceBackward;
    }

    void init(Ui::ModularGraph *a_ui) {
        QSettings settings(tr("MusicalGraph"), tr("MusicalGraph"), parent);
        if(settings.contains(tr("sound")))
            sound = settings.value(tr("sound")).toBool();
        else sound = true;
        if(settings.contains(tr("gameVersion"))){
            gameVersion = settings.value(tr("gameVersion")).toString();
            cerr << "gameVersion=" << gameVersion.toStdString() << endl;}
        else {
            settings.setValue(tr("gameVersion"), tr("easy"));
            gameVersion = tr("easy");
        }
        if(settings.contains(gameVersion+tr("problemIndex")))
            problemIndex = settings.value(gameVersion+tr("problemIndex")).toInt();
        else problemIndex = 0;
        if(settings.contains(gameVersion+tr("modulus")))
            modulus = settings.value(gameVersion+tr("modulus")).toInt();
        else if(gameVersion == tr("hard"))
            modulus = 84;
        else
            modulus = 12;
        ui = a_ui;
        commonInit();
        makeGraph();
        value = valueDist(gen);
        target = pickTarget();

        finishScene();
        ((ModularGraph *) parent)->emitValueChanged();
        ((ModularGraph *) parent)->emitTargetChanged();
        ((ModularGraph *) parent)->emitModulusChanged();
        ((ModularGraph *) parent)->emitProblemIndexChanged();
        if(gameVersion == tr("easy")) {
            parent->addClock();
            parent->initTime();
            nextMoveHint();
        }
    }

    void init(Ui::ModularGraph *a_ui, int a_modulus, int a_problemIndex) {
        ui = a_ui;
        modulus = a_modulus;
        problemIndex = a_problemIndex;
        ((ModularGraph *) parent)->emitProblemIndexChanged();

        commonInit();
        value = valueDist(gen);
        target = pickTarget();
        finishScene();
    }

    void commonInit()
    {

        QSettings settings(tr("MusicalGraph"), tr("MusicalGraph"), parent);
        if(settings.contains(tr("count")))
            settings.setValue(tr("count"), settings.value(tr("count")).toInt()+1);
        else
            settings.setValue(tr("count"), 0);


        start = std::chrono::steady_clock::now();
        melody.clear();
        valueDist = std::uniform_int_distribution<int>(0, modulus-1);
        gen.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        moves = 0;
        melody.push_back(value);

        Node * n0 = new Node(parentNode, -50000, -5000, 0);
        n0->activate();
        n0->setOpacity(0.0);
        itemBuffer.push_back(n0);
        Node * n1 = new Node(parentNode, -50000, 5000, 0);
        n1->activate();
        n1->setOpacity(0.0);
        itemBuffer.push_back(n1);
        Node * n2 = new Node(parentNode, 50000, -5000, 0);
        n2->activate();
        n2->setOpacity(0.0);
        itemBuffer.push_back(n2);
        Node * n3 = new Node(parentNode, 50000, 5000, 0);
        n3->activate();
        n3->setOpacity(0.0);
        itemBuffer.push_back(n3);
    }

    void makeTraps() {
        std::uniform_int_distribution<int> nodeIndexDist(0, nodes.size()-1);
        for(int i = 0; i < nodes.size()/10; i++)
            nodes.at(nodeIndexDist(gen))->setValueTrap(modulus);
    }

    int pickTarget() {

        std::set<int> values;
        std::vector<std::pair<int, Node*>> heads;

        heads.push_back(std::make_pair(value, activeNode));

        int currentDepth = 0;
        while(++currentDepth<20 && values.size()<modulus-1) {
            std::vector<std::pair<int, Node*>> newHeads;
            for(auto it = heads.cbegin(); it != heads.cend(); it++) {
                if(values.find(it->first) == values.end() && (values.size()>=modulus-2 || currentDepth>=50)) {
#ifdef DEBUG
                    cerr << "values.find(newValue) == values.end()? " << to_string(values.find(it->first) == values.end()) << endl;
                    cerr << "values.size()="<<values.size() << endl;
                    cerr << "currentDepth=" << currentDepth << endl;
                    cerr << "requiredDepth=" << std::to_string(currentDepth) << endl;
#endif
                    return it->first;
                }
                values.insert(it->first);
                std::vector<Node *> neighbors = it->second->getNeighbors();
                for(auto innerIt = neighbors.cbegin(); innerIt != neighbors.cend(); innerIt++)
                    newHeads.push_back(std::make_pair((it->first+(*innerIt)->value())%modulus, *innerIt));
            }
            heads = newHeads;
        }

        std::vector<Node *> neighbors = activeNode->getNeighbors();
        Node *neighbor = neighbors.front();
        return (value+neighbor->value())%modulus;
    }

    Node *nextNode() {
        std::vector<std::pair<int, std::vector<Node*>>> walks;
        walks.push_back(std::make_pair(value, std::vector<Node *>({activeNode})));
        int currentDepth = 0;
        while(++currentDepth < 10) {
            std::vector<std::pair<int, std::vector<Node*>>> newWalks;
            for(auto it = walks.begin(); it != walks.end(); it++) {
                int walkValue = it->first;
                std::vector<Node *> walk = it->second;
                Node *head = walk.back();
                std::vector<Node *> neighbors = head->getNeighbors();

                for(auto neighbor = neighbors.cbegin(); neighbor != neighbors.cend(); neighbor++) {
                    std::vector<Node *> newWalk = std::vector<Node *>(walk);
                    newWalk.push_back(*neighbor);
                    int newValue = (walkValue+(*neighbor)->value()) % modulus;
                    if(newValue == target && *(newWalk.begin()+1) != activeNode) return *(newWalk.begin()+1);
                    newWalks.push_back(std::make_pair(newValue, newWalk));
                }
            }
            walks = newWalks;
        }
        return nullptr;
    }

    bool fewestMoves() {
        std::vector<std::pair<int, std::vector<Node*>>> walks;
        walks.push_back(std::make_pair(value, std::vector<Node *>({activeNode})));
        int currentDepth = 0;
        while(++currentDepth < 20) {
            std::vector<std::pair<int, std::vector<Node*>>> newWalks;
            for(auto it = walks.begin(); it != walks.end(); it++) {
                int walkValue = it->first;
                std::vector<Node *> walk = it->second;
                Node *head = walk.back();
                std::vector<Node *> neighbors = head->getNeighbors();
                for(auto neighbor = neighbors.cbegin(); neighbor != neighbors.cend(); neighbor++) {
                    std::vector<Node *> newWalk = std::vector<Node *>(walk);
                    newWalk.push_back(*neighbor);
                    int newValue = (walkValue+(*neighbor)->value()) % modulus;
                    if(newValue == target && *(newWalk.begin()+1) != activeNode) {
                        requiredDepth=currentDepth;
                        return true;
                    }
                    newWalks.push_back(std::make_pair(newValue, newWalk));
                }
            }
            walks = newWalks;
        }
        return false;
    }

    void nextMoveHint() {
        for(auto it = nodes.cbegin(); it != nodes.cend(); it++)
            (*it)->setRedglow(false);
        nextNodePtr = nextNode();
        if(nextNodePtr != nullptr) nextNodePtr->setRedglow();
        else nextNodePtr = parentNode;
    }

    void finishScene() {
        for(auto it = itemBuffer.cbegin(); it != itemBuffer.cend(); it++) {
            ui->graphicsView->scene()->addItem(*it);
          //  if(std::find(nodes.cbegin(), nodes.cend(), *it) ==  nodes.cend())
                nodes.push_back((Node *) *it);
        }
        if(!scaleSet) {
            QRectF rect;
            rect.setBottom(0); rect.setTop(0); rect.setRight(0); rect.setLeft(0);
            auto items = ui->graphicsView->scene()->items();
            for(auto it = items.cbegin(); it != items.cend(); it++)
                if(abs((*it)->zValue()-NODE_Z_VALUE)<.01 || abs((*it)->zValue()-EDGE_Z_VALUE)<.01)
                    if((*it)->y() < rect.bottom()) rect.setBottom((*it)->y());
                    else if((*it)->y() > rect.top()) rect.setTop((*it)->y());
                    else if((*it)->x() < rect.left()) rect.setLeft((*it)->x());
                    else if((*it)->x() > rect.right()) rect.setRight((*it)->x());
            double padding = rect.width()>rect.height() ? 0.4*rect.width() : 0.2*rect.height();
            rect.setBottom(rect.bottom()-padding);
            rect.setTop(rect.top()+padding);
            rect.setLeft(rect.left()-padding);
            rect.setRight(rect.right()+padding);
            scaleSet = true;
            ui->graphicsView->setSceneRect(rect);
            ui->graphicsView->centerOn(ui->graphicsView->mapFromScene(QPoint(0,0)));
        }

        activeNodeDanceForward = new QPropertyAnimation(activeNode, "radius", nullptr);
        activeNodeDanceBackward = new QPropertyAnimation(activeNode, "radius", nullptr);
        activeNodeDanceForward->setStartValue(1);
        activeNodeDanceForward->setEndValue(1.4);
        activeNodeDanceForward->setEasingCurve(QEasingCurve::InOutElastic);
        activeNodeDanceForward->setKeyValueAt(0.5, 1.2);
        activeNodeDanceForward->setDuration(2500);

        activeNodeDanceBackward->setStartValue(1.4);
        activeNodeDanceBackward->setEndValue(1);
        activeNodeDanceBackward->setEasingCurve(QEasingCurve::InOutElastic);
        activeNodeDanceBackward->setKeyValueAt(0.5, 1.2);
        activeNodeDanceBackward->setDuration(2500);
        QObject::connect(activeNodeDanceForward, SIGNAL(finished()), activeNodeDanceBackward, SLOT(start()));
        QObject::connect(activeNodeDanceBackward, SIGNAL(finished()), activeNodeDanceForward, SLOT(start()));

        makeActiveNodeDance();

        gameplay.insert(tr("startingValue"), value);
        gameplay.insert(tr("targetValue"), target);
        gameplay.insert(tr("startingNode"), static_cast<long long>(activeNode->getId()));

        if(modulus>12)
            for(auto it = nodes.cbegin(); it != nodes.cend(); it++) {
                (*it)->setAlwaysDrawNumbers();
                (*it)->setPixmapFromValue();
            }

        makeTraps();
        int i = 0;
        while(!fewestMoves() && ++i < 3) {
            target = pickTarget();
        }
        cumulative+=requiredDepth;
    }

    void makeActiveNodeDance() {
        QObject::disconnect(activeNodeDanceForward, SIGNAL(finished()), activeNodeDanceBackward, SLOT(start()));
        QObject::disconnect(activeNodeDanceBackward, SIGNAL(finished()), activeNodeDanceForward, SLOT(start()));

        activeNodeDanceForward->stop();
        activeNodeDanceBackward->stop();

        activeNodeDanceForward->setTargetObject(activeNode);
        activeNodeDanceBackward->setTargetObject(activeNode);

        for(auto it = nodes.cbegin(); it != nodes.cend(); it++) {
            (*it)->setScale(1);
            (*it)->update();
        }

        activeNodeDanceForward->start();
        QObject::connect(activeNodeDanceForward, SIGNAL(finished()), activeNodeDanceBackward, SLOT(start()));
        QObject::connect(activeNodeDanceBackward, SIGNAL(finished()), activeNodeDanceForward, SLOT(start()));
    }

    void makeGraph() {
#ifdef DEBUG
        cerr << "In makeGraph(). gameVersion=" << gameVersion.toStdString() << endl;
#endif
        edgeSet = new EdgeSet(parentNode);

        bool named = false;

        if(problemIndex>33) {
            int x = valueDist(gen);
            if(x%modulus/3==2) {
                makeNodes_nGon();
#ifdef DEBUG
        cerr << "In makeGraph(). makeNodes_nGon()" << endl;
#endif
            } else if(x%modulus/3==1) {
                makeBigLattice(); named = true;
#ifdef DEBUG
        cerr << "In makeGraph(). makeBigLattice()" << endl;
#endif
            } else {
                makeTree(); named = true;
#ifdef DEBUG
        cerr << "In makeGraph(). makeTree()" << endl;
#endif
            }
        } else {
            switch(problemIndex+3 % 45) {
            case 3:  make3Nodes(); break;
            case 4:  make4Nodes(); break;
            case 5:  make5Nodes(); break;
            case 6:  makeNodes_Shrikhande(); named = true; break;
            case 11: makeNodes_grotzsch(); named = true; break;
            case 12: makeNodes_Shrikhande(); named = true; break;
            case 14: makeNodes_TreeIntoTriPartite0(); named = true; break;
            case 16: case 17: case 18: case 19: case 20:
                named = true; makeBigLattice(); break;
            case 21: makeNodes_TreeIntoTriPartite1(); named = true; break;
            case 22: makeNodes_TreeIntoTriPartite2(); named = true; break;
            case 23: makeNodes_TreeIntoTriPartite3(); named = true; break;
            case 24: case 25: case 26:
                makeNodes_TreeIntoTriPartite4(); named = true; break;
            case 27: case 28: case 29: case 30:
                makeNodes_TreeIntoTriPartite5(); named = true; break;
            case 31: case 32:
                makeNodes_web0(); named = true; break;
            case 33: case 34:
                makeNodes_web1(); named = true; break;
            default:
                makeNodes_nGon();
                break;

            }


        }
#ifdef DEBUG
        cerr << "problemIndex = " << problemIndex << endl;
#endif
        if(!named)
            makeRandomEdges();
        for(auto it = nodes.cbegin(); it != nodes.cend(); it++)
            itemBuffer.push_back(*it);
        itemBuffer.push_back(edgeSet);
        (*nodes.cbegin())->activate();
        activeNode = *nodes.begin();
    }

    void makeBigLattice() {
        for(int i = -2; i < 3; i++)
            for(int j = -2; j < 3; j++) {
                Node *n = new Node(parentNode, i*1000, j*1000, valueDist(gen));
                nodes.push_back(n);
            }

        std::uniform_int_distribution<int> coin(0, 1);

        int numberOfNodesPerRowAndCol = 5;
        for(int i = 0; i < numberOfNodesPerRowAndCol; i++)
            for(int j = 0; j < numberOfNodesPerRowAndCol; j++) {
                int flip1 = coin(gen);
                int flip2 = coin(gen);
                int flip3 = coin(gen);
                int flip4 = coin(gen);
                if(i>0 && flip1==1)
                    connect(nodes.at(i*numberOfNodesPerRowAndCol+j), nodes.at((i-1)*numberOfNodesPerRowAndCol+j));
                if(j>0 && flip2==1)
                    connect(nodes.at(i*numberOfNodesPerRowAndCol+j), nodes.at(i*numberOfNodesPerRowAndCol+j-1));
                if(i*numberOfNodesPerRowAndCol+j+1<nodes.size() && (flip3==1 || (flip1 == 0 && flip2 == 0)))
                    connect(nodes.at(i*numberOfNodesPerRowAndCol+j), nodes.at(i*numberOfNodesPerRowAndCol+j+1));
                if((i+1)*numberOfNodesPerRowAndCol+j<nodes.size() && (flip4==1 || (flip1 == 0 && flip2 == 0 && flip3 == 0)))
                    connect(nodes.at(i*numberOfNodesPerRowAndCol+j), nodes.at((i+1)*numberOfNodesPerRowAndCol+j));
            }

        (*nodes.cbegin())->activate();
        activeNode = *nodes.begin();
        if(activeNode->getNeighbors().size() == 0) connect(activeNode, nodes.at(1));
        ui->graphicsView->scene()->update();
    }

    void make3Nodes() {

        Node *n1 = new Node(parentNode, edge_scalingFactor*(-500+x_shift),  edge_scalingFactor*y_shift, valueDist(gen));
        n1->activate();
        Node *n2 = new Node(parentNode,   edge_scalingFactor*(500+x_shift),  edge_scalingFactor*y_shift, valueDist(gen));
        Node *n3 = new Node(parentNode,     edge_scalingFactor*x_shift,   edge_scalingFactor*(500+y_shift), valueDist(gen));
        activeNode = n1;

        nodes = {n1, n2, n3};
    }

    void make4Nodes() {
        Node *n1 = new Node(parentNode,  edge_scalingFactor*(-500+x_shift),  edge_scalingFactor*y_shift, valueDist(gen));
        n1->activate(); activeNode = n1;
        Node *n2 = new Node(parentNode,   edge_scalingFactor*(500+x_shift),  edge_scalingFactor*y_shift, valueDist(gen));
        Node *n3 = new Node(parentNode,       edge_scalingFactor*x_shift,   edge_scalingFactor*(500+y_shift), valueDist(gen));
        Node *n4 = new Node(parentNode,       edge_scalingFactor*x_shift,  edge_scalingFactor*(-500+y_shift), valueDist(gen));

        nodes = {n1, n2, n3, n4};
    }

    void make5Nodes() {
        Node *n1 = new Node(parentNode,  edge_scalingFactor*(-750+x_shift),  edge_scalingFactor*y_shift, valueDist(gen));
        n1->activate();
        activeNode = n1;
        Node *n2 = new Node(parentNode,   edge_scalingFactor*(750+x_shift),  edge_scalingFactor*y_shift, valueDist(gen));
        Node *n3 = new Node(parentNode,       edge_scalingFactor*x_shift,   edge_scalingFactor*(750+y_shift), valueDist(gen));
        Node *n4 = new Node(parentNode,  edge_scalingFactor*(-500+x_shift),  edge_scalingFactor*(-750+y_shift), valueDist(gen));
        Node *n5 = new Node(parentNode,   edge_scalingFactor*(500+x_shift),  edge_scalingFactor*(-750+y_shift), valueDist(gen));

        nodes = {n1, n2, n3, n4, n5};
    }

    void makeNodes_grotzsch() {
        for(int i = 0; i < 11; i++)
            nodes.push_back(new Node(parentNode, grotzsch_embedding[i].x(), grotzsch_embedding[i].y(), valueDist(gen)));
        (*nodes.cbegin())->activate();
        activeNode = *nodes.cbegin();

        connect(nodes.at(0), nodes.at(7)); connect(nodes.at(0), nodes.at(4)); connect(nodes.at(0), nodes.at(1));
        connect(nodes.at(0), nodes.at(10)); connect(nodes.at(2), nodes.at(7)); connect(nodes.at(2), nodes.at(9));
        connect(nodes.at(2), nodes.at(1)); connect(nodes.at(2), nodes.at(3)); connect(nodes.at(4), nodes.at(9));
        connect(nodes.at(4), nodes.at(5)); connect(nodes.at(4), nodes.at(3)); connect(nodes.at(7), nodes.at(5));
        connect(nodes.at(7), nodes.at(8)); connect(nodes.at(9), nodes.at(8)); connect(nodes.at(9), nodes.at(10));
        connect(nodes.at(6), nodes.at(1)); connect(nodes.at(6), nodes.at(3)); connect(nodes.at(6), nodes.at(5));
        connect(nodes.at(6), nodes.at(8)); connect(nodes.at(6), nodes.at(10));

    }

    void makeNodes_Shrikhande() {
        for(int i = 0; i < 16; i++)
            nodes.push_back(new Node(parentNode, shrikhande_embedding[i].x(), shrikhande_embedding[i].y(), valueDist(gen)));
        (*nodes.cbegin())->activate();
        activeNode = *nodes.cbegin();

        connect(nodes.at(0), nodes.at(1)); connect(nodes.at(0), nodes.at(2));
        connect(nodes.at(1), nodes.at(2)); connect(nodes.at(0), nodes.at(3));
        connect(nodes.at(1), nodes.at(3)); connect(nodes.at(1), nodes.at(6));
        connect(nodes.at(1), nodes.at(7)); connect(nodes.at(1), nodes.at(14));
        connect(nodes.at(6), nodes.at(14)); connect(nodes.at(7), nodes.at(14));
        connect(nodes.at(13), nodes.at(15)); connect(nodes.at(12), nodes.at(15));
        connect(nodes.at(15), nodes.at(0)); connect(nodes.at(0), nodes.at(9));

        connect(nodes.at(12), nodes.at(14)); connect(nodes.at(13), nodes.at(14));
        connect(nodes.at(14), nodes.at(15)); connect(nodes.at(9), nodes.at(15));
        connect(nodes.at(8), nodes.at(15)); connect(nodes.at(0), nodes.at(8));

        connect(nodes.at(4), nodes.at(6)); connect(nodes.at(4), nodes.at(2));
        connect(nodes.at(4), nodes.at(5)); connect(nodes.at(4), nodes.at(8));
        connect(nodes.at(4), nodes.at(12)); connect(nodes.at(4), nodes.at(10));

        connect(nodes.at(5), nodes.at(2)); connect(nodes.at(5), nodes.at(9));
        connect(nodes.at(5), nodes.at(11)); connect(nodes.at(5), nodes.at(12));
        connect(nodes.at(5), nodes.at(7));

        connect(nodes.at(10), nodes.at(3)); connect(nodes.at(10), nodes.at(8));
        connect(nodes.at(10), nodes.at(11)); connect(nodes.at(10), nodes.at(13));
        connect(nodes.at(10), nodes.at(6));

        connect(nodes.at(11), nodes.at(3)); connect(nodes.at(11), nodes.at(13));
        connect(nodes.at(11), nodes.at(7)); connect(nodes.at(11), nodes.at(9));
    }

    void makeTree() {
        std::uniform_int_distribution<int> treeDist(0, 6);
        switch(treeDist(gen)) {
        case 0: makeNodes_TreeIntoTriPartite0(); break;
        case 1: makeNodes_TreeIntoTriPartite1(); break;
        case 2: makeNodes_TreeIntoTriPartite2(); break;
        case 3: makeNodes_TreeIntoTriPartite3(); break;
        case 4: makeNodes_TreeIntoTriPartite4(); break;
        default: makeNodes_TreeIntoTriPartite5(); break;
        }
    }

    void makeNodes_TreeIntoTriPartite0() {
        for(int i = 0; i < 22; i++)
            nodes.push_back(new Node(parentNode, tree_embedding0[i].x(), tree_embedding0[i].y(), valueDist(gen)));
        (*nodes.cbegin())->activate();
        activeNode = *nodes.cbegin();

        connect(nodes.at(0), nodes.at(1)); connect(nodes.at(0), nodes.at(2));
        connect(nodes.at(2), nodes.at(4)); connect(nodes.at(2), nodes.at(6));
        connect(nodes.at(1), nodes.at(5)); connect(nodes.at(1), nodes.at(3));
        connect(nodes.at(1), nodes.at(15)); connect(nodes.at(15), nodes.at(7));
        connect(nodes.at(3), nodes.at(7)); connect(nodes.at(5), nodes.at(21));
        connect(nodes.at(6), nodes.at(16)); connect(nodes.at(4), nodes.at(14));
        connect(nodes.at(4), nodes.at(18)); connect(nodes.at(4), nodes.at(20));
        connect(nodes.at(7), nodes.at(19)); connect(nodes.at(7), nodes.at(13));
        connect(nodes.at(21), nodes.at(13)); connect(nodes.at(16), nodes.at(8));
        connect(nodes.at(16), nodes.at(17)); connect(nodes.at(14), nodes.at(17));
        connect(nodes.at(14), nodes.at(10)); connect(nodes.at(18), nodes.at(10));
        connect(nodes.at(18), nodes.at(12)); connect(nodes.at(20), nodes.at(10));
        connect(nodes.at(9), nodes.at(10)); connect(nodes.at(8), nodes.at(9));
        connect(nodes.at(11), nodes.at(13)); connect(nodes.at(11), nodes.at(19));

    }

    void makeNodes_TreeIntoTriPartite1() {
        for(int i = 0; i < 27; i++)
            nodes.push_back(new Node(parentNode, tree_embedding1[i].x(), tree_embedding1[i].y(), valueDist(gen)));
        (*nodes.cbegin())->activate();
        activeNode = *nodes.cbegin();

        connect(nodes.at(0), nodes.at(1)); connect(nodes.at(0), nodes.at(2));
        connect(nodes.at(2), nodes.at(4)); connect(nodes.at(2), nodes.at(6));
        connect(nodes.at(1), nodes.at(5)); connect(nodes.at(1), nodes.at(3));
        connect(nodes.at(1), nodes.at(15)); connect(nodes.at(15), nodes.at(7));
        connect(nodes.at(3), nodes.at(7)); connect(nodes.at(5), nodes.at(21));
        connect(nodes.at(6), nodes.at(16)); connect(nodes.at(4), nodes.at(14));
        connect(nodes.at(4), nodes.at(18)); connect(nodes.at(4), nodes.at(20));
        connect(nodes.at(7), nodes.at(19)); connect(nodes.at(7), nodes.at(13));
        connect(nodes.at(21), nodes.at(13)); connect(nodes.at(16), nodes.at(8));
        connect(nodes.at(16), nodes.at(17)); connect(nodes.at(14), nodes.at(17));
        connect(nodes.at(14), nodes.at(10)); connect(nodes.at(18), nodes.at(10));
        connect(nodes.at(18), nodes.at(12)); connect(nodes.at(20), nodes.at(10));
        connect(nodes.at(9), nodes.at(10)); connect(nodes.at(8), nodes.at(9));
        connect(nodes.at(11), nodes.at(13)); connect(nodes.at(11), nodes.at(19));
        connect(nodes.at(9), nodes.at(21)); connect(nodes.at(9), nodes.at(22));
        connect(nodes.at(9), nodes.at(23));
        connect(nodes.at(11), nodes.at(23)); connect(nodes.at(11), nodes.at(24));
        connect(nodes.at(11), nodes.at(25)); connect(nodes.at(11), nodes.at(26));

    }

    void makeNodes_TreeIntoTriPartite2() {
        for(int i = 0; i < 16; i++)
            nodes.push_back(new Node(parentNode, tree_embedding2[i].x(), tree_embedding2[i].y(), valueDist(gen)));
        (*nodes.cbegin())->activate();
        activeNode = *nodes.cbegin();

        connect(nodes.at(0), nodes.at(1)); connect(nodes.at(0), nodes.at(2));
        connect(nodes.at(0), nodes.at(3)); connect(nodes.at(1), nodes.at(4));
        connect(nodes.at(1), nodes.at(5)); connect(nodes.at(1), nodes.at(6));
        connect(nodes.at(2), nodes.at(5)); connect(nodes.at(2), nodes.at(6));
        connect(nodes.at(2), nodes.at(7)); connect(nodes.at(3), nodes.at(4));
        connect(nodes.at(3), nodes.at(5)); connect(nodes.at(3), nodes.at(8));
        connect(nodes.at(4), nodes.at(9)); connect(nodes.at(4), nodes.at(10));
        connect(nodes.at(4), nodes.at(11)); connect(nodes.at(4), nodes.at(12));
        connect(nodes.at(5), nodes.at(10)); connect(nodes.at(5), nodes.at(11));
        connect(nodes.at(5), nodes.at(12)); connect(nodes.at(5), nodes.at(13));
        connect(nodes.at(6), nodes.at(11)); connect(nodes.at(6), nodes.at(12));
        connect(nodes.at(6), nodes.at(13)); connect(nodes.at(6), nodes.at(14));
        connect(nodes.at(7), nodes.at(12)); connect(nodes.at(7), nodes.at(13));
        connect(nodes.at(7), nodes.at(14)); connect(nodes.at(7), nodes.at(15));
        connect(nodes.at(8), nodes.at(13)); connect(nodes.at(8), nodes.at(14));

    }

    void makeNodes_TreeIntoTriPartite3() {
        for(int i = 0; i < 20; i++)
            nodes.push_back(new Node(parentNode, tree_embedding3[i].x(), tree_embedding3[i].y(), valueDist(gen)));
        (*nodes.cbegin())->activate();
        activeNode = *nodes.cbegin();

        connect(nodes.at(0), nodes.at(1)); connect(nodes.at(0), nodes.at(2));
        connect(nodes.at(0), nodes.at(3)); connect(nodes.at(1), nodes.at(4));
        connect(nodes.at(1), nodes.at(5)); connect(nodes.at(1), nodes.at(6));
        connect(nodes.at(2), nodes.at(5)); connect(nodes.at(2), nodes.at(6));
        connect(nodes.at(2), nodes.at(7)); connect(nodes.at(3), nodes.at(4));
        connect(nodes.at(3), nodes.at(5)); connect(nodes.at(3), nodes.at(8));
        connect(nodes.at(4), nodes.at(9)); connect(nodes.at(4), nodes.at(10));
        connect(nodes.at(4), nodes.at(11)); connect(nodes.at(4), nodes.at(12));
        connect(nodes.at(5), nodes.at(10)); connect(nodes.at(5), nodes.at(11));
        connect(nodes.at(5), nodes.at(12)); connect(nodes.at(5), nodes.at(13));
        connect(nodes.at(6), nodes.at(11)); connect(nodes.at(6), nodes.at(12));
        connect(nodes.at(6), nodes.at(13)); connect(nodes.at(6), nodes.at(14));
        connect(nodes.at(7), nodes.at(12)); connect(nodes.at(7), nodes.at(13));
        connect(nodes.at(7), nodes.at(14)); connect(nodes.at(7), nodes.at(15));
        connect(nodes.at(8), nodes.at(13)); connect(nodes.at(8), nodes.at(14));

        connect(nodes.at(10), nodes.at(16));
        connect(nodes.at(11), nodes.at(17));
        connect(nodes.at(12), nodes.at(17)); connect(nodes.at(12), nodes.at(18));
        connect(nodes.at(13), nodes.at(18)); connect(nodes.at(13), nodes.at(19));

    }


    void makeNodes_TreeIntoTriPartite4() {
        for(int i = 0; i < 24; i++)
            nodes.push_back(new Node(parentNode, tree_embedding4[i].x(), tree_embedding4[i].y(), valueDist(gen)));
        (*nodes.cbegin())->activate();
        activeNode = *nodes.cbegin();

        connect(nodes.at(0), nodes.at(1)); connect(nodes.at(0), nodes.at(2));
        connect(nodes.at(0), nodes.at(3)); connect(nodes.at(1), nodes.at(4));
        connect(nodes.at(1), nodes.at(5)); connect(nodes.at(1), nodes.at(6));
        connect(nodes.at(2), nodes.at(5)); connect(nodes.at(2), nodes.at(6));
        connect(nodes.at(2), nodes.at(7)); connect(nodes.at(3), nodes.at(4));
        connect(nodes.at(3), nodes.at(5)); connect(nodes.at(3), nodes.at(8));
        connect(nodes.at(4), nodes.at(9)); connect(nodes.at(4), nodes.at(10));
        connect(nodes.at(4), nodes.at(11)); connect(nodes.at(4), nodes.at(12));
        connect(nodes.at(5), nodes.at(10)); connect(nodes.at(5), nodes.at(11));
        connect(nodes.at(5), nodes.at(12)); connect(nodes.at(5), nodes.at(13));
        connect(nodes.at(6), nodes.at(11)); connect(nodes.at(6), nodes.at(12));
        connect(nodes.at(6), nodes.at(13)); connect(nodes.at(6), nodes.at(14));
        connect(nodes.at(7), nodes.at(12)); connect(nodes.at(7), nodes.at(13));
        connect(nodes.at(7), nodes.at(14)); connect(nodes.at(7), nodes.at(15));
        connect(nodes.at(8), nodes.at(13)); connect(nodes.at(8), nodes.at(14));

        connect(nodes.at(10), nodes.at(16));
        connect(nodes.at(11), nodes.at(17));
        connect(nodes.at(12), nodes.at(17)); connect(nodes.at(12), nodes.at(18));
        connect(nodes.at(13), nodes.at(18)); connect(nodes.at(13), nodes.at(19));

        connect(nodes.at(16), nodes.at(20)); connect(nodes.at(17), nodes.at(23));
        connect(nodes.at(17), nodes.at(21));
        connect(nodes.at(18), nodes.at(22));
        connect(nodes.at(19), nodes.at(23));

    }
    void makeNodes_TreeIntoTriPartite5() {
        for(int i = 0; i < 28; i++)
            nodes.push_back(new Node(parentNode, tree_embedding5[i].x(), tree_embedding5[i].y(), valueDist(gen)));
        (*nodes.cbegin())->activate();
        activeNode = *nodes.cbegin();

        connect(nodes.at(0), nodes.at(1)); connect(nodes.at(0), nodes.at(2));
        connect(nodes.at(0), nodes.at(3)); connect(nodes.at(1), nodes.at(4));
        connect(nodes.at(1), nodes.at(5)); connect(nodes.at(1), nodes.at(6));
        connect(nodes.at(2), nodes.at(5)); connect(nodes.at(2), nodes.at(6));
        connect(nodes.at(2), nodes.at(7)); connect(nodes.at(3), nodes.at(4));
        connect(nodes.at(3), nodes.at(5)); connect(nodes.at(3), nodes.at(8));
        connect(nodes.at(4), nodes.at(9)); connect(nodes.at(4), nodes.at(10));
        connect(nodes.at(4), nodes.at(11)); connect(nodes.at(4), nodes.at(12));
        connect(nodes.at(5), nodes.at(10)); connect(nodes.at(5), nodes.at(11));
        connect(nodes.at(5), nodes.at(12)); connect(nodes.at(5), nodes.at(13));
        connect(nodes.at(6), nodes.at(11)); connect(nodes.at(6), nodes.at(12));
        connect(nodes.at(6), nodes.at(13)); connect(nodes.at(6), nodes.at(14));
        connect(nodes.at(7), nodes.at(12)); connect(nodes.at(7), nodes.at(13));
        connect(nodes.at(7), nodes.at(14)); connect(nodes.at(7), nodes.at(15));
        connect(nodes.at(8), nodes.at(13)); connect(nodes.at(8), nodes.at(14));

        connect(nodes.at(10), nodes.at(16));
        connect(nodes.at(11), nodes.at(17));
        connect(nodes.at(12), nodes.at(17)); connect(nodes.at(12), nodes.at(18));
        connect(nodes.at(13), nodes.at(18)); connect(nodes.at(13), nodes.at(19));

        connect(nodes.at(16), nodes.at(20)); connect(nodes.at(17), nodes.at(23));
        connect(nodes.at(17), nodes.at(21));
        connect(nodes.at(18), nodes.at(22));
        connect(nodes.at(19), nodes.at(23));


        connect(nodes.at(24), nodes.at(20)); connect(nodes.at(24), nodes.at(23));
        connect(nodes.at(25), nodes.at(21));
        connect(nodes.at(26), nodes.at(22));
        connect(nodes.at(27), nodes.at(23));

    }

    void makeNodes_web0() {
        for(int i = 0; i < 14; i++)
            nodes.push_back(new Node(parentNode, web0[i].x(), web0[i].y(), valueDist(gen)));
        (*nodes.cbegin())->activate();
        activeNode = *nodes.cbegin();

        connect(nodes.at(0), nodes.at(1));
        connect(nodes.at(1), nodes.at(2));
        connect(nodes.at(2), nodes.at(3));
        connect(nodes.at(3), nodes.at(4));
        connect(nodes.at(4), nodes.at(5));
        connect(nodes.at(5), nodes.at(0));
        connect(nodes.at(6), nodes.at(7));
        connect(nodes.at(7), nodes.at(8));
        connect(nodes.at(8), nodes.at(9));
        connect(nodes.at(9), nodes.at(10));
        connect(nodes.at(10), nodes.at(11));
        connect(nodes.at(11), nodes.at(12));
        connect(nodes.at(12), nodes.at(13));
        connect(nodes.at(13), nodes.at(6));

        connect(nodes.at(2), nodes.at(10));

        if(valueDist(gen) < modulus/2)
            connect(nodes.at(0), nodes.at(6));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(0), nodes.at(7));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(1), nodes.at(7));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(1), nodes.at(8));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(2), nodes.at(9));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(3), nodes.at(10));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(3), nodes.at(11));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(4), nodes.at(11));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(4), nodes.at(12));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(5), nodes.at(13));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(5), nodes.at(6));

    }

    void makeNodes_web1() {
        for(int i = 0; i < 14; i++)
            nodes.push_back(new Node(parentNode, web0[i].x(), web0[i].y(), valueDist(gen)));
        (*nodes.cbegin())->activate();
        activeNode = *nodes.cbegin();

        connect(nodes.at(0), nodes.at(1));
        connect(nodes.at(1), nodes.at(2));
        connect(nodes.at(2), nodes.at(3));
        connect(nodes.at(3), nodes.at(4));
        connect(nodes.at(4), nodes.at(5));
        connect(nodes.at(5), nodes.at(0));
        connect(nodes.at(6), nodes.at(7));
        connect(nodes.at(7), nodes.at(8));
        connect(nodes.at(8), nodes.at(9));
        connect(nodes.at(9), nodes.at(10));
        connect(nodes.at(10), nodes.at(11));
        connect(nodes.at(11), nodes.at(12));
        connect(nodes.at(12), nodes.at(13));
        connect(nodes.at(13), nodes.at(6));
        connect(nodes.at(2), nodes.at(10));

        if(valueDist(gen) < modulus/2)
            connect(nodes.at(0), nodes.at(6));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(0), nodes.at(7));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(1), nodes.at(7));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(1), nodes.at(8));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(2), nodes.at(9));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(3), nodes.at(10));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(3), nodes.at(11));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(4), nodes.at(11));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(4), nodes.at(12));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(5), nodes.at(13));
        if(valueDist(gen) < modulus/2)
            connect(nodes.at(5), nodes.at(6));

        // Add more stuffs here

    }

    void makeNodes_hexagonalLattice() {
        int spacing = 800;
        int numberOfRows = 9;
        int numberOfCols = 11;
        int nodeCounter = 0;
        for(int row = 0; row<numberOfRows; row++)
            for(int col = 0; col<numberOfCols; col++) {
                if(row%2==0) {
                    nodes.push_back(new Node(parentNode, -3500+col*spacing+0.5*spacing, -3500+row*spacing, valueDist(gen)));
                    nodeCounter++;
                } else if(col%2==0) {
                    nodes.push_back(new Node(parentNode, -3500+col*spacing, -3500+row*spacing, valueDist(gen)));
                    nodeCounter++;
                }
            }
        (*nodes.cbegin())->activate();
        activeNode = *nodes.cbegin();

    }

    void makeNodes_nGon() {
        makeNodes_nGon(problemIndex);
    }

    void makeNodes_nGon(int n) {
        n = std::min(n, 15);
        if(n<3) return;
        for(int index = 0; index < n; index++) {
            int r = 3000;
            Node *node = new Node(nullptr,  edge_scalingFactor*(r*cos(3.141592*(2.0*index+n)/n)+x_shift),  edge_scalingFactor*(r*sin(3.141592*(2.0*index+n)/n)+y_shift), valueDist(gen));
            nodes.push_back(node);
        }
        (*nodes.cbegin())->activate();
        activeNode = *nodes.cbegin();
    }

    void makeRandomEdges() {
        if(problemIndex == 3) {
            std::uniform_int_distribution<int> dist(0, problemIndex);
            switch(dist(gen)) {
            case 0: connect(nodes.at(0), nodes.at(1));
                connect(nodes.at(1), nodes.at(2));
                break;
            case 1: connect(nodes.at(1), nodes.at(2));
                connect(nodes.at(2), nodes.at(0));
                break;
            case 2: connect(nodes.at(0), nodes.at(1));
                connect(nodes.at(2), nodes.at(0));
                break;
            case 3: connect(nodes.at(0), nodes.at(1));
                connect(nodes.at(1), nodes.at(2));
                connect(nodes.at(2), nodes.at(0));
                break;
            }
        } else {
            std::uniform_int_distribution<int> randEdgeSetCardinality(nodes.size(), nodes.size()*(nodes.size()-1)/2);
            std::uniform_int_distribution<int> randIndex(0, nodes.size()-1);
            std::uniform_int_distribution<int> randIndexMinusOne(0, nodes.size()-2);

            int edgeSetCardinality = randEdgeSetCardinality(gen);
            int numberOfEdges = 0;

            for(size i = 1; i < nodes.size(); i++) {
                std::uniform_int_distribution<int> randAvailableIndex(0, i);
                int safety = 0;
                while(!connect(nodes.at(randAvailableIndex(gen)), nodes.at(i)) && ++safety<10000);
                numberOfEdges++;
            }

            while(numberOfEdges++ < edgeSetCardinality) {
                std::vector<int> indices;
                size i = randIndex(gen);
                size j = 0;
                for(; j < i; j++)
                    if(!nodes.at(i)->isConnected(nodes.at(j)))
                        indices.push_back(j);
                j++;
                for(; j < nodes.size(); j++)
                    if(!nodes.at(i)->isConnected(nodes.at(j)))
                        indices.push_back(j);
                if(indices.size() == 0) continue;
                std::uniform_int_distribution<int> randAvailableIndex(0, indices.size()-1);
                int randomIndex = randAvailableIndex(gen);
                connect(nodes.at(i), nodes.at(indices.at(randomIndex)));
            }
        }
    }

    Node *neighborTo(QGraphicsItem *item)
    {
        if(!item) return nullptr;
        if(abs(item->zValue() - NODE_Z_VALUE)>0.1) return nullptr;
        Node *result = (Node *) item;
        if(!activeNode->isConnected(result))
            return nullptr;
        return result;
    }

    void selectNode(Node *n)
    {
        n->squish();
        n->setValueTrap(0);
        if(gameVersion == QObject::tr("easy"))
            parent->initTime();
        value += n->value();
        fewestMoves();
        parent->emitScoreChanged();
        value %= modulus;
        if(sound) {
            parent->emitNote(value);
        }
        parent->emitDelta(n->value());
        activeNode->inactivate();
        activeNode->setPixmapFromValue();
        activeNode = n;
        makeActiveNodeDance();
        n->activate();
        n->setPixmapFromValue();
        n->setRedglow(false);
        melody.push_back(value);

        moveList.push_back(static_cast<long long>(n->getId()));

        if(gameWon()) {
            if(++targetCounter<10) {
                target = pickTarget();
                int i = 0;
                while(!fewestMoves() && ++i < 2) {
                    target = pickTarget();
                }
                cumulative+=requiredDepth;
                parent->emitTargetChanged();
                parent->advanceProgressBar();

            } else {
                int duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count();

                QJsonObject jsonObject;
                jsonObject.insert(tr("level"), QJsonValue::fromVariant(problemIndex));
                jsonObject.insert(tr("order"), QJsonValue::fromVariant(static_cast<int>(nodes.size())));
                jsonObject.insert(tr("modulus"), QJsonValue::fromVariant(modulus));
                jsonObject.insert(tr("moves"), QJsonValue::fromVariant(moves));
                jsonObject.insert(tr("duration"), QJsonValue::fromVariant(duration));
                jsonObject.insert(tr("minimumMoves"), QJsonValue::fromVariant(cumulative));

                int denominator = cumulative<1 ? 1 : cumulative;
                double s = (double(moves)/double(denominator)+duration/denominator)/2.0;
                jsonObject.insert(tr("s"), QJsonValue::fromVariant(s));

                QJsonArray melodyArray;
                for(auto it = melody.cbegin(); it != melody.cend(); it++)
                    melodyArray.push_back(*it);
                jsonObject.insert(tr("melody"), melodyArray);

                QSettings settings(tr("MusicalGraph"), tr("MusicalGraph"), parent);
                settings.setValue(gameVersion+tr("problemIndex"), ++problemIndex);

                int gameVersionTrials = 0;
                if(settings.contains(gameVersion+tr("Trials")))
                    gameVersionTrials = settings.value(gameVersion+tr("Trials")).toInt();

                double s_mu = s;
                if(settings.contains(gameVersion+tr("00S")))
                    s_mu = settings.value(gameVersion+tr("00S")).toDouble();

                if(std::abs(s-s_mu)>0.001)
                    s_mu = double(gameVersionTrials)/double(gameVersionTrials+1)*s_mu+s/double(gameVersionTrials+1);

                settings.setValue(gameVersion+tr("00S"), s_mu);

                settings.setValue(gameVersion+tr("Trials"), gameVersionTrials+1);

                int trials = 0;
                if(settings.contains(tr("trials")))
                    trials = settings.value(tr("trials")).toInt();

                s_mu = s;
                if(settings.contains(tr("00S")))
                    s_mu = settings.value(tr("00S")).toDouble();

                if(std::abs(s-s_mu)>0.001)
                    s_mu = double(trials)/double(trials+1)*s_mu+s/double(trials+1);

                settings.setValue(tr("00S"), s_mu);

                settings.setValue(tr("trials"), trials+1);


                if(gameVersion==tr("medium")) {
                    if(problemIndex%10==0 && modulus<=84) {
                        modulus += 6;
                        settings.setValue(gameVersion+tr("modulus"), modulus);
                    }
                }

                if(gameVersion==tr("easy")) {
                    if(problemIndex%(modulus+2)==0 && modulus<12) {
                        modulus++;
                        settings.setValue(gameVersion+tr("modulus"), modulus);
                    }
                }

                parent->endGame(jsonObject);
#ifdef BACKEND_FUNCTIONALITY
                sendGame();
#endif
                cleanUp();
            }
        }
        if(gameVersion == tr("easy") && problemIndex < 200 && targetCounter < 2) nextMoveHint();
    }

    void cleanUp() {
        for(auto it = nodes.cbegin(); it != nodes.cend(); it++)
            ui->graphicsView->scene()->removeItem(*it);
        ui->graphicsView->scene()->removeItem(edgeSet);
        nodes.clear();
        edgeSet->clear();
        targetCounter = 0;
        requiredDepth = 0;
        cumulative = 0;
    }

    bool connect(Node *n1, Node* n2)
    {
        if(n1 == nullptr || n2 == nullptr) return false;
        edgeSet->addEdge(n1, n2);
        return n1->connect(n2) && n2->connect(n1);
    }

    void graphicsClicked(QPointF point) {
        QGraphicsItem *item = ui->graphicsView->scene()->itemAt(point, parent->scoreboard->deviceTransform(ui->graphicsView->viewportTransform()));
        if(!item) return;
#ifdef DEBUG
        cerr << "something clicked item->zValue=" << std::to_string(item->zValue()) << endl;
#endif
        Node *n = neighborTo(item);
        if(n == nullptr) return;
        if(!n->getSelectable()) return;
        moves++;
        selectNode(n);
        ((ModularGraph *) parent)->emitMovesChanged();
        ((ModularGraph *) parent)->emitValueChanged();
        ui->graphicsView->scene()->update();
        for(auto it = nodes.cbegin(); it != nodes.cend(); it++)
            (*it)->update();
    }

    int getMoves() {
        return moves;
    }

    bool gameWon() {
        return target == value;
    }

};

ModularGraph::ModularGraph(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModularGraph)
{
    ui->setupUi(this);
    hide();

    GraphicsScene *scene = new GraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setViewport(new QOpenGLWidget);

    PImpl = std::unique_ptr<Impl>(new ModularGraph::Impl(this));

    connect(scene, SIGNAL(click(const QPointF&)), this, SLOT(graphicsClicked(const QPointF&)));

    scoreboard = new Scoreboard(PImpl->parentNode);
    movmodboard = new MovModBoard(PImpl->parentNode);
    progressBar = new ProgressBar(PImpl->parentNode);
    sBoard = new SBoard(PImpl->parentNode);
}

ModularGraph::~ModularGraph()
{
#ifdef DEBUG
        cerr << "Destroy ModularGraph" << endl;
#endif
    delete ui;
}

void ModularGraph::init()
{
#ifdef DEBUG
        cerr << "In ModularGraph::init()" << endl;
#endif

    PImpl->init(ui);

    ui->graphicsView->scene()->addItem(scoreboard);
    scoreboard->setTarget(PImpl->target);
    scoreboard->setValue(PImpl->value);
    scoreboard->moveToCorner();

    ui->graphicsView->scene()->addItem(movmodboard);
    movmodboard->setModulus(PImpl->modulus);
    movmodboard->setMoves(PImpl->moves);
    movmodboard->moveToCorner();

    ui->graphicsView->scene()->addItem(progressBar);
    progressBar->setProgress(1);
    progressBar->moveToCorner();

    ui->graphicsView->scene()->addItem(sBoard);

    ui->graphicsView->setScoreboard(scoreboard);

    QObject::connect(this, SIGNAL(targetChanged(int)), scoreboard, SLOT(setTarget(int)));
    QObject::connect(this, SIGNAL(valueChanged(int)), scoreboard, SLOT(setValue(int)));
    QObject::connect(ui->graphicsView, SIGNAL(move()), scoreboard, SLOT(moveToCorner()));
    QObject::connect(ui->graphicsView, &GraphicsView::showMenu, this, &ModularGraph::emitDisplayMenu);

    QObject::connect(this, SIGNAL(modulusChanged(int)), movmodboard, SLOT(setModulus(int)));
    QObject::connect(this, SIGNAL(movesChanged(int)), movmodboard, SLOT(setMoves(int)));
    QObject::connect(this, SIGNAL(problemIndexChanged(int)), movmodboard, SLOT(setProblemIndex(int)));
    QObject::connect(ui->graphicsView, SIGNAL(move()), movmodboard, SLOT(moveToCorner()));

    QObject::connect(ui->graphicsView, &GraphicsView::move, progressBar, &ProgressBar::moveToCorner);

    QObject::connect(ui->graphicsView, &GraphicsView::move, sBoard, &SBoard::moveToCorner);
    QObject::connect(this, &ModularGraph::scoreChanged, sBoard, &SBoard::setScore);

    emitProblemIndexChanged();

}

void ModularGraph::init(int modulus, int order)
{
#ifdef DEBUG
        cerr << "In ModularGraph::init(int modulus, int order)" << endl;
#endif
    PImpl->init(ui, modulus, order);
    ui->graphicsView->centerOn(QPointF(x_shift, y_shift));
#ifdef DEBUG
        cerr << "Leaving ModularGraph::init(int modulus, int order)" << endl;
#endif
}

bool ModularGraph::gameWon()
{
    return PImpl->gameWon();
}

int ModularGraph::getMoves()
{
    return PImpl->getMoves();
}

int ModularGraph::getTarget()
{
    return PImpl->target;
}

void ModularGraph::emitModulusChanged()
{
    emit modulusChanged(PImpl->modulus);
}

void ModularGraph::emitTargetChanged()
{
    emit targetChanged(PImpl->target);
}


void ModularGraph::emitMovesChanged()
{
    emit movesChanged(PImpl->moves);
    movmodboard->setMoves(PImpl->moves);
}

void ModularGraph::emitValueChanged()
{
    emit valueChanged(PImpl->value);
}

void ModularGraph::emitScoreChanged()
{
    int duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - PImpl->start).count();
    int denominator = PImpl->cumulative<1 ? 1 : PImpl->cumulative;
    double s = (PImpl->moves/denominator+duration/denominator)/2.0;
    emit scoreChanged(s);
}

void ModularGraph::emitProblemIndexChanged()
{
    emit problemIndexChanged(PImpl->problemIndex);
}

void ModularGraph::emitNote(int value)
{
    emit note(value);
}

void ModularGraph::emitDelta(int value)
{
    emit delta(value);
}

void ModularGraph::emitDisplayMenu()
{
#ifdef DEBUG
        cerr << "In void ModularGraph::emitDisplaySettings()" << endl;
#endif
    emit displayMenu();
}

void ModularGraph::emitStepSignal(int steps)
{
    emit stepSignal(steps);
}

void ModularGraph::advanceProgressBar()
{
    progressBar->setProgress(progressBar->getProgress()+1);
}

void ModularGraph::addClock()
{
    if(clock == nullptr)
        clock = new Clock(PImpl->parentNode);
    ui->graphicsView->scene()->addItem(clock);
    clock->moveToCorner();
    QObject::connect(ui->graphicsView, &GraphicsView::move, clock, &Clock::moveToCorner);
    QObject::connect(this, &ModularGraph::delta, clock, &Clock::timeTravel);
    QObject::connect(this, &ModularGraph::modulusChanged, clock, &Clock::setModulus);
}

void ModularGraph::removeClock()
{
    if(clock != nullptr) {
        ui->graphicsView->scene()->removeItem(clock);
        QObject::disconnect(ui->graphicsView, &GraphicsView::move, clock, &Clock::moveToCorner);
        QObject::disconnect(this, &ModularGraph::modulusChanged, clock, &Clock::setModulus);
    }
}

void ModularGraph::initTime()
{
    clock->setModulus(PImpl->modulus);
    clock->setRadianDelta();
    clock->setTime(PImpl->value);
}

void ModularGraph::startMultiPlayerGame()
{
    PImpl->gametype = GAMETYPE::MULTIPLAYER;
    PImpl->cleanUp();
    init();
}

void ModularGraph::startSinglePlayerGame()
{
    PImpl->gametype = GAMETYPE::SINGLEPLAYER;
    PImpl->cleanUp();
    init();
}

Scoreboard *ModularGraph::getScoreboard() const
{
    return scoreboard;
}

void ModularGraph::endGame(QJsonObject json)
{
    emit gameOver(json);
}

void ModularGraph::graphicsClicked(const QPointF& point)
{
    PImpl->graphicsClicked(point);
}
